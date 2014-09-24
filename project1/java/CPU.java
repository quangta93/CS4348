
import java.util.*;

public class CPU {
	
	private int programCounter = 0;
	private int stackCounter = 0;
	private int instructionCounter = 0;
	private int accumulatorCounter = 0;
	private int x = 0;
	private int y = 0;
	private int[] memory = new int[2000];

	public CPU() {
		this.stackCounter = Memory.MEMORY_SIZE - 1;
	}

	public void fetch() {

	}

	public void execute() {

	}
}