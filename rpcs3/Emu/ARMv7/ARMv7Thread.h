#pragma once
#include "Emu/CPU/CPUThread.h"

enum ARMv7InstructionSet
{
	ARM,
	Thumb,
	Jazelle,
	ThumbEE,
};

class ARMv7Thread : public CPUThread
{
public:
	ARMv7Thread();

	union
	{
		u32 GPR[15];

		struct
		{
			u32 pad[13];

			union
			{
				u32 SP;

				struct { u16 SP_main, SP_process; };
			};

			u32 LR;
		};
	};

	union
	{
		struct
		{
			u32 N : 1; //Negative condition code flag
			u32 Z : 1; //Zero condition code flag
			u32 C : 1; //Carry condition code flag
			u32 V : 1; //Overflow condition code flag
			u32 Q : 1; //Set to 1 if an SSAT or USAT instruction changes (saturates) the input value for the signed or unsigned range of the result
			u32 : 27;
		};

		u32 APSR;

	} APSR;

	union
	{
		struct
		{
			u32 : 24;
			u32 exception : 8;
		};

		u32 IPSR;

	} IPSR;

	ARMv7InstructionSet ISET;

	union
	{
		struct
		{
			u8 cond : 3;
			u8 state : 5;
		};

		u8 IT;

		u32 advance()
		{
			const u32 res = (state & 0xf) ? (cond << 1 | state >> 4) : 0xe /* true */;

			state <<= 1;
			if ((state & 0xf) == 0) // if no d
			{
				IT = 0; // clear ITSTATE
			}

			return res;
		}

		operator bool() const
		{
			return (state & 0xf) != 0;
		}

	} ITSTATE;

	void write_gpr(u32 n, u32 value)
	{
		assert(n < 16);

		if(n < 15)
		{
			GPR[n] = value;
		}
		else
		{
			SetBranch(value & ~1);
		}
	}

	u32 read_gpr(u32 n)
	{
		assert(n < 16);

		if(n < 15)
		{
			return GPR[n];
		}
		
		return PC;
	}

public:
	virtual void InitRegs(); 
	virtual void InitStack();
	u32 GetStackArg(u32 pos);

public:
	virtual std::string RegsToString();
	virtual std::string ReadRegString(const std::string& reg);
	virtual bool WriteRegString(const std::string& reg, std::string value);

protected:
	virtual void DoReset();
	virtual void DoRun();
	virtual void DoPause();
	virtual void DoResume();
	virtual void DoStop();

	virtual void DoCode();
};
