#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/IR/Module.h"

using namespace llvm;

namespace {
  struct InstrumentPass : public FunctionPass {
    static char ID;
    InstrumentPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &fun) override {
      LLVMContext &ctx = fun.getContext();

      for (auto &block : fun) {
        for (auto &inst : block) {
          if (auto *op = dyn_cast<LoadInst>(&inst)) {
            IRBuilder<> builder(op);
            builder.SetInsertPoint(&block, ++builder.GetInsertPoint()); // Insert call after load

            // Insert function declaration using the correct LLVM pointer type for op. "print" takes
            // type void *.
            Constant *logFunc = fun.getParent()->getOrInsertFunction(
              "print", Type::getVoidTy(ctx), op->getPointerOperand()->getType(), NULL
            );

            Value* args[] = {op->getPointerOperand()};
            builder.CreateCall(logFunc, args);
          }
        }
      }

      return true;
    }
  };
}

char InstrumentPass::ID = 0;

static RegisterPass<InstrumentPass> tmp("instrument", "Instrument loop load/stores");
