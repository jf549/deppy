#include <tracer/tracerdefs.h>

#include "llvm/Pass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

#include <stack>

using namespace llvm;

namespace {
  // An LLVM pass to instrument loop events (loop entry, iteration and exit)
  struct LoopInstrumentPass : public FunctionPass {
    static char ID;
    LoopInstrumentPass() : FunctionPass(ID) {}

    void getAnalysisUsage(AnalysisUsage& AU) const override {
      // Require LoopInfoWrapperPass so that LoopInfo is available to this pass
      AU.addRequired<LoopInfoWrapperPass>();
    }

    void instrumentLoop(Loop* loop, LLVMContext& ctx, Constant* loopEventFn) const {
      IRBuilder<> callBuilder(ctx);

      // Insert a LoopEntry event into the loop's preheader basic block, just before the
      // terminator to avoid prematurely announcing loop entry
      if (auto preheader = loop->getLoopPreheader()) {
        callBuilder.SetInsertPoint(preheader->getTerminator());
        callBuilder.CreateCall(loopEventFn, ConstantInt::get(Type::getInt8Ty(ctx), LOOP_ENTRY));
      } else {
        errs() << "Failed to get preheader (must be in LoopSimplify form)\n";
      }

      // Insert a LoopIter event into the loop's latch block
      if (auto latch = loop->getLoopLatch()) {
        callBuilder.SetInsertPoint(latch, latch->getFirstInsertionPt());
        callBuilder.CreateCall(loopEventFn, ConstantInt::get(Type::getInt8Ty(ctx), LOOP_ITER));
      } else {
        errs() << "Failed to get loop latch (must be in LoopSimplify form)\n";
      }

      // Insert a LoopExit event into the beginning of each of the loop's exit blocks
      if (loop->hasDedicatedExits()) {
        SmallVector<BasicBlock*, 1> exitBlocks; // TODO tune initial size
        loop->getUniqueExitBlocks(exitBlocks);

        for (auto exitBlock : exitBlocks) {
          callBuilder.SetInsertPoint(exitBlock, exitBlock->getFirstInsertionPt());
          callBuilder.CreateCall(loopEventFn, ConstantInt::get(Type::getInt8Ty(ctx), LOOP_EXIT));
        }
      } else {
         errs() << "Loop does not have dedicated exits (must be in LoopSimplify form)\n";
      }
    }

    // For each (sub)loop in the function, insert a call to the dynamically linked loopEvent()
    // function into the loop's preheader (entry), latch (iteration) and exit blocks (exit).
    // Nested loops are traversed in depth-first order using an explicit stack.
    // NB: This pass requires all loops to be in LoopSimplify form so that every loop has a unique
    // preheader and latch, as well as dedicated exit blocks.
    virtual bool runOnFunction(Function& fn) override {
      auto& ctx = fn.getContext();
      const auto& loopInfo = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
      auto loopEventFn = fn.getParent()->
        getOrInsertFunction("loopEvent", Type::getVoidTy(ctx), Type::getInt8Ty(ctx), nullptr);
      std::stack<Loop*> loopStack;

      for (auto loop : loopInfo) {
        loopStack.push(loop);
      }

      while (!loopStack.empty()) {
        auto loop = loopStack.top();
        loopStack.pop();

        for (auto subloop : *loop) {
          loopStack.push(subloop);
        }

        if (loop->isLoopSimplifyForm()) {
          instrumentLoop(loop, ctx, loopEventFn);
        } else {
          errs() << "Loop not in LoopSimplify form, skipping\n";
        }
      }

      return true; // Have modified function
    }
  };
}

char LoopInstrumentPass::ID = 0;
static RegisterPass<LoopInstrumentPass> tmp("instrument-loops", "Instrument loop events");
