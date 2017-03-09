#include "llvm/Pass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"

#include <stack>

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
    void instrumentLoopEvents(Function& fun) const {
      auto& ctx = fun.getContext();
      const auto& loopInfo = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
      auto loopEventFun = fun.getParent()->
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
          callBuilder.SetInsertPoint(preheader, preheader->getFirstInsertionPt()); // TODO need to insert into last position, not first
          callBuilder.CreateCall(loopEventFun, ConstantInt::get(Type::getInt32Ty(ctx), 0)); // TODO put into template fun
        } else {
          errs() << "Failed to get preheader\n";
        }

        // Insert a LoopIter event into the latch
        if (auto latch = loop->getLoopLatch()) {
          callBuilder.SetInsertPoint(latch, latch->getFirstInsertionPt());
          callBuilder.CreateCall(loopEventFun, ConstantInt::get(Type::getInt32Ty(ctx), 1));
        } else {
          errs() << "Failed to get loop latch\n";
        }

        // Insert a LoopExit event into each exit block
        if (loop->hasDedicatedExits()) {
          SmallVector<BasicBlock*, 1> exitBlocks;
          loop->getUniqueExitBlocks(exitBlocks);
          for (auto exitBlock : exitBlocks) {
            callBuilder.SetInsertPoint(exitBlock, exitBlock->getFirstInsertionPt());
            callBuilder.CreateCall(loopEventFun, ConstantInt::get(Type::getInt32Ty(ctx), 2));
          }
        } else {
           errs() << "Loop must have dedicated exits (put in LoopSimplify form)\n";
        }
      }
    }

    void instrumentMemoryEvents(Function& fun) const {
      auto& ctx = fun.getContext();

      // Next we perform a peephole analysis over each instruction in the function. For every load
      // or store instruction, we insert a function call to our dynamic library. The function is
      // provided with (1) whether the memory instruction is a store or load, (2) the address
      // accessed and (3) the line number of the instruction in the IR.
      for (auto& block : fun) {
        for (auto& inst : block) {
          if (isa<LoadInst>(inst) || isa<StoreInst>(inst)) {
            Constant* libFun; // Runtime library function to call on load/store
            Value* args[2];
            IRBuilder<> builder(&inst);
            builder.SetInsertPoint(&block, ++builder.GetInsertPoint()); // Insert call after op

            if (auto loadInst = dyn_cast<LoadInst>(&inst)) {
              // Insert function declaration using the correct LLVM pointer type for op. "print"
              // takes type void *.
              libFun = fun.getParent()->getOrInsertFunction(
                "print", Type::getVoidTy(ctx), Type::getInt32Ty(ctx),
                loadInst->getPointerOperand()->getType(), nullptr
              );

              // TODO have two functions for load and store??
              args[0] = ConstantInt::get(Type::getInt32Ty(ctx), 1); // isLoad == 1
              args[1] = loadInst->getPointerOperand();
              // TODO line number

            } else {
              auto storeInst = cast<StoreInst>(&inst);
              libFun = fun.getParent()->getOrInsertFunction(
                "print", Type::getVoidTy(ctx), Type::getInt32Ty(ctx),
                storeInst->getPointerOperand()->getType(), nullptr
              );

              args[0] = ConstantInt::get(Type::getInt32Ty(ctx), 0); // isLoad == 0
              args[1] = storeInst->getPointerOperand();
              // TODO line number
            }

            builder.CreateCall(libFun, args);
          }
        }
      }
    }

    virtual bool runOnFunction(Function& fun) override {
      instrumentMemoryEvents(fun);
      instrumentLoopEvents(fun);

      return true; // We have modified the function
    }
  };
}

char InstrumentPass::ID = 0;
static RegisterPass<InstrumentPass> tmp("instrument", "Instrument loop load/stores");
