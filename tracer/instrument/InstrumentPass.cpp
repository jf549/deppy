#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
  struct InstrumentPass : public FunctionPass {
    static char ID;
    InstrumentPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &fun) override {
      LLVMContext &ctx = fun.getContext();

      for (auto &block : fun) {
        for (auto &inst : block) {
          if (isa<LoadInst>(&inst) || isa<StoreInst>(&inst)) {
            Constant *libFun; // Runtime library function to call on load/store
            Value *args[2];
            IRBuilder<> builder(&inst);
            builder.SetInsertPoint(&block, ++builder.GetInsertPoint()); // Insert call after op

            if (auto *op = dyn_cast<LoadInst>(&inst)) {
              // Insert function declaration using the correct LLVM pointer type for op. "print"
              // takes type void *.
              libFun = fun.getParent()->getOrInsertFunction(
                "print", Type::getVoidTy(ctx), Type::getInt32Ty(ctx),
                op->getPointerOperand()->getType(), NULL
              );

              args[0] = ConstantInt::get(Type::getInt32Ty(ctx), 1); // isLoad == 1
              args[1] = op->getPointerOperand();

            } else if (auto *op = dyn_cast<StoreInst>(&inst)) {
              libFun = fun.getParent()->getOrInsertFunction(
                "print", Type::getVoidTy(ctx), Type::getInt32Ty(ctx),
                op->getPointerOperand()->getType(), NULL
              );

              args[0] = ConstantInt::get(Type::getInt32Ty(ctx), 0); // isLoad == 0
              args[1] = op->getPointerOperand();
            } else {
              continue;
            }

            builder.CreateCall(libFun, args);
          }
        }
      }

      return true;
    }
  };
}

char InstrumentPass::ID = 0;
static RegisterPass<InstrumentPass> tmp("instrument", "Instrument loop load/stores");
