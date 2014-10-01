
import java.util.*;

public class CPU {

	private final int TIMER_INTERRUPT_ADDRESS = 1000;
	private final int INSTRUCTION_INTERRUPT_ADDRESS = 1500;

	private final int TIMER_INTERRUPT_CODE = 1;
	private fianl int INSTRUCTION_INTERRUPT_CODE = 2;
	
	private int programCounter = 0;
	private int stackPointer = 0;
	private int instructionRegister = 0;	// store the instruction at (programCounter - 1)
	private int accumulatorCounter = 0;
	private int x = 0;			// timer causes interrupt after every x instructions
	private int y = 0;

	private boolean interruptEnabled = true;
	private int timer = 0;
	private Process memory = null;

	public CPU(int x) {
		this.stackPointer = Memory.SYSTEM_CODE_START - 1;
		this.x = x;
	}

	public void addMemory(Process proc) {
		this.memory = proc;
	}

	/* Fetches instruction at PC address to IR */
	public void fetch() throws Exception {
		if (this.memory == null) return;	// haven't add Memory process
		
		PrintWriter writer = new PrintWriter(this.memory.getOutputStream());
		writer.print("r" + programCounter);		// read at PC address
		writer.flush();
		writer.close();

		// save content to IR
		
		programCounter ++;
	}

	public void execute() {

	}

	private void interruptHandler(int interruptCode) {
		// save registers into system stack; order = [PC, SP, IR, AC]
		int systemStackCounter = Memory.MEMORY_SIZE - 1;

		if (interruptCode == TIMER_INTERRUPT_CODE) {

		} else if (interruptCode == INSTRUCTION_INTERRUPT_CODE) {

		}
		/*
			switch to system stack
			save registers to system stack
			timer --> address 1000; instruction --> address 1500
			interruptEnabled = false;
		*/
	}

	public static void main(String args[]) {
		/* Checks input */
		if (args == null && args.length != 2) {
			System.out.println();
		}

		String filename = args[0];
		int x = Integer.parseInt(args[1]);
		CPU processor = new CPU(x);

		try {
			Runtime rt = Runtime.getRuntime();
			Process proc = rt.exec("java Memory " + filename);
			processor.addMemory(proc);

			while (true) {
				processor.fetch();
				processor.execute();
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}