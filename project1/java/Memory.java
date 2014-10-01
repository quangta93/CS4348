
import java.util.*;
import java.io.*;

public class Memory {

	public static final int MEMORY_SIZE = 2000;
	public static final int SYSTEM_CODE_START = 1000;

	private int[] storage = null;

	public Memory(String filename) throws Exception {
		this.storage = new int[MEMORY_SIZE];
		initialize(filename);
	}

	private void initialize(String filename) throws Exception {
		try {
			Scanner scanner = new Scanner(new File(filename), "UTF-8");
			int counter = 0;
			while (scanner.hasNext()) {
				String line = scanner.nextLine();
				line = line.split(" ")[0];
				if (!line.equals("")) {
					// not blank line
					if (line.charAt(0) == '.') {
						// position
						counter = Integer.parseInt(line.substring(1));
					} else {
						// data or instruction
						storage[counter++] = Integer.parseInt(line);
					}
				}
			}
			scanner.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	/* Returns the value at the address */
	public int read(int address) {
		return 0;
	}

	/* Writes data to the address */
	public void write(int address, int data) {
		// TODO: check whether data is valid to write into address
		storage[address] = data;
	}

	/* Checks if the current address if in system code */
	private boolean isSystemCode(int pointer) {
		return (pointer > 999) && (pointer < 2000);
	}

	/* Checks if the current address if in user code */
	private boolean isUserCode(int pointer) {
		return (0 < pointer) && (pointer < 1000);
	}

	public static void main(String args[]) throws Exception {
		if (args == null || args.length == 0) System.exit(0);
		Memory memory = new Memory(args[0]);

		Scanner input = new Scanner(System.in);
	}
}