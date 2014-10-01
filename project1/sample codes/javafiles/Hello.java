
import java.util.*;
import java.io.*;

public class Hello implements Runnable
{
	private Scanner scanner = null;

	public static void main(String[] args) throws Exception {
		Scanner inputStream = new Scanner(System.in);
		Hello h = new Hello(inputStream);
		h.start();
		inputStream.close();
	}

	Hello(Scanner scanner) {
		this.scanner = scanner;
	}

	private void start() {
		new Thread(this).start();
	}

	public void run() {
		if (this.scanner != null && this.scanner.hasNext()) {
			String line = scanner.nextLine().trim();
			if (line.equalsIgnoreCase("9")) {
				System.out.print(line);
				return;
			}
			System.out.print("from child: " + line);
		} else {
			try {
				// Thread.sleep(500);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}
}
