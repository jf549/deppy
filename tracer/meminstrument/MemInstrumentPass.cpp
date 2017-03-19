#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

#define LOAD 0
#define STORE 1

using namespace llvm;

namespace {
  // An LLVM pass to instrument memory events (load and store instruction)
  struct MemInstrumentPass : public FunctionPass {
    static char ID;
    MemInstrumentPass() : FunctionPass(ID) {}

    // Insert a function call just before a load or store instruction. Using function template to
    // workaround lack of load/store base class, without duplicating code.
    template<typename InstT>
    void insertCall(LLVMContext& ctx, InstT* inst, Constant* fnToCall) const {
      IRBuilder<> callBuilder(inst); // Insert function call before instruction
      Value* args[3] = {
        ConstantInt::get(Type::getInt8Ty(ctx), std::is_same<InstT, LoadInst>::value ? LOAD : STORE),
        callBuilder.CreateBitCast(inst->getPointerOperand(), Type::getInt8PtrTy(ctx)),
        ConstantInt::get(Type::getInt64Ty(ctx), reinterpret_cast<uintptr_t>(inst))
      };
      callBuilder.CreateCall(fnToCall, args);
    }

    // Perform a peephole analysis over each instruction in the function. For every load or store
    // instruction seen, insert a call to the dynamically linked memoryEvent() function, which
    // takes parameters (1) the type of the memory access (load or store), (2) a pointer to the
    // address accessed and (3) the ID of the load/store instruction.
    virtual bool runOnFunction(Function& fn) override {
      auto& ctx = fn.getContext();
      auto memoryEventFn = fn.getParent()->getOrInsertFunction("memoryEvent", Type::getVoidTy(ctx),
        Type::getInt8Ty(ctx), Type::getInt8PtrTy(ctx), Type::getInt64Ty(ctx), nullptr);

      for (auto& block : fn) {
        for (auto& inst : block) {
          if (auto loadInst = dyn_cast<LoadInst>(&inst)) {
            insertCall(ctx, loadInst, memoryEventFn);
          } else if (auto storeInst = dyn_cast<StoreInst>(&inst)) {
            insertCall(ctx, storeInst, memoryEventFn);
          }
        }
      }

      return true; // Have modified function
    }
  };
}

char MemInstrumentPass::ID = 0;
static RegisterPass<MemInstrumentPass> tmp("instrument-mem", "Instrument loads and stores");
