#include "llvm/Pass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"

#include <stack>

#define LOOP_ENTRY 0
#define LOOP_ITER 1
#define LOOP_EXIT 2
#define LOAD 0
#define STORE 1

using namespace llvm;

namespace {
  struct InstrumentPass : public FunctionPass {
    static char ID;
    InstrumentPass() : FunctionPass(ID) {}

    void getAnalysisUsage(AnalysisUsage& AU) const override {
      // Require LoopInfoWrapperPass so that LoopInfo is available to this pass
      AU.addRequired<LoopInfoWrapperPass>();
    }

    // For each loop in the function, we emit a loop event for each loop entry, iteration and
    // exit. This is achieved by inserting a function call to our dynamic library into the loop's
    // preheader, latch and exit blocks.
    void instrumentLoopEvents(Function& fn) const {
      auto& ctx = fn.getContext();
      const auto& loopInfo = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
      auto loopEventFn = fn.getParent()->
        getOrInsertFunction("loopEvent", Type::getVoidTy(ctx), Type::getInt32Ty(ctx), nullptr);
      IRBuilder<> callBuilder(ctx);
      std::stack<Loop*> loopStack;

      // Push top level loops onto the stack
      for (auto loop : loopInfo) {
        loopStack.push(loop);
      }

      while (!loopStack.empty()) {
        auto loop = loopStack.top();
        loopStack.pop();

        // Push subloops onto the stack
        for (auto subloop : *loop) {
          loopStack.push(subloop);
        }

        // Check that the loop has been put in LoopSimplfy form
        if (!loop->isLoopSimplifyForm()) {
          errs() << "Loop not in LoopSimplify form, skipping\n";
          continue;
        }

        // Insert a LoopEntry event into the preheader
        if (auto preheader = loop->getLoopPreheader()) {
          callBuilder.SetInsertPoint(preheader->getTerminator());
          callBuilder.CreateCall(loopEventFn, ConstantInt::get(Type::getInt32Ty(ctx), LOOP_ENTRY));
        } else {
          errs() << "Failed to get preheader\n";
        }

        // Insert a LoopIter event into the latch
        if (auto latch = loop->getLoopLatch()) {
          callBuilder.SetInsertPoint(latch, latch->getFirstInsertionPt());
          callBuilder.CreateCall(loopEventFn, ConstantInt::get(Type::getInt32Ty(ctx), LOOP_ITER));
        } else {
          errs() << "Failed to get loop latch\n";
        }

        // Insert a LoopExit event into each exit block
        if (loop->hasDedicatedExits()) {
          SmallVector<BasicBlock*, 1> exitBlocks; // TODO tune this
          loop->getUniqueExitBlocks(exitBlocks);
          for (auto exitBlock : exitBlocks) {
            callBuilder.SetInsertPoint(exitBlock, exitBlock->getFirstInsertionPt());
            callBuilder.CreateCall(loopEventFn, ConstantInt::get(Type::getInt32Ty(ctx), LOOP_EXIT));
          }
        } else {
           errs() << "Loop must have dedicated exits (put in LoopSimplify form)\n";
        }
      }
    }

    template<typename InstT>
    void insertCall(LLVMContext& ctx, InstT* inst, Constant* fnToCall) const {
      IRBuilder<> builder(inst); // Insert function call before instruction
      Value* args[3]; // TODO use SmallVector?
      args[0] = ConstantInt::get(Type::getInt32Ty(ctx), std::is_same<InstT, LoadInst>::value ? LOAD : STORE);
      args[1] = builder.CreateBitCast(inst->getPointerOperand(), Type::getInt8PtrTy(ctx));
      args[2] = ConstantInt::get(Type::getInt64Ty(ctx), reinterpret_cast<uintptr_t>(inst));
      builder.CreateCall(fnToCall, args);
    }

    // Next we perform a peephole analysis over each instruction in the function. For every load
    // or store instruction, we insert a function call to our dynamic library. The function is
    // provided with (1) whether the memory instruction is a store or load, (2) the address
    // accessed and (3) the line number of the instruction in the IR.
    void instrumentMemoryEvents(Function& fn) const {
      auto& ctx = fn.getContext();
      // Runtime library function to call on load/store
      auto memoryEventFn = fn.getParent()->getOrInsertFunction("memoryEvent", Type::getVoidTy(ctx),
        Type::getInt32Ty(ctx), Type::getInt8PtrTy(ctx), Type::getInt64Ty(ctx), nullptr);

      for (auto& block : fn) {
        for (auto& inst : block) {
          if (auto loadInst = dyn_cast<LoadInst>(&inst)) {
            insertCall(ctx, loadInst, memoryEventFn);
          } else if (auto storeInst = dyn_cast<StoreInst>(&inst)) {
            insertCall(ctx, storeInst, memoryEventFn);
          }
        }
      }
    }

    virtual bool runOnFunction(Function& fn) override {
      instrumentMemoryEvents(fn);
      instrumentLoopEvents(fn);

      return true; // We have modified the function
    }
  };
}

char InstrumentPass::ID = 0;
static RegisterPass<InstrumentPass> tmp("instrument", "Instrument loop load/stores");
