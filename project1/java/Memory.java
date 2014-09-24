
import java.util.*;

public class Memory {

	public static final int MEMORY_SIZE = 2000;
	private final int SYSTEM_CODE_START = 1000;
	private int[] storage = null;

	public Memory(String filename) {
		this.storage = new int[MEMORY_SIZE];
		initialize(filename);
	}

	private void initialize(String filename) {
		Scanner scanner = new Scanner(new File(filename), "UTF-8");
		int counter = 0;
		while (scanner.hasNextLine()) {
			try {
				int data = Integer.parseInt(scanner.nextLine().split(" ")[0]);
				storage[counter++] = data;
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		scanner.close();
	}

	/* Returns the value at the address */
	public int read(int address) {

	}

	/* Writes data to the address */
	public void write(int address, int data) {

	}
}