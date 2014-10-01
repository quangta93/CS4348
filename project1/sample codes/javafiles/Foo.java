import java.util.*;
import java.io.*;

public class Foo {

	public static void main(String[] args) throws Exception {

		Runtime rt = Runtime.getRuntime();
		Process process = rt.exec("java Hello");	// start child process
		PrintWriter pw = new PrintWriter(process.getOutputStream());
		InputStream is = process.getInputStream();
		long startTime = System.currentTimeMillis();
		int counter = 0;
		boolean ack = true;

		while (true) {
			if (ack) {
				// send next counter to child process if the previous counter is received back
				System.out.println("from parent: " + counter);
				pw.println(counter);
				if (counter < 10) counter ++;	// stop counter at 9
				pw.flush();
				ack = false;
			}

			// keep listening for input from child process
			long currentTime = System.currentTimeMillis();
			if (currentTime - startTime > 5000) {
				// if time exceeds 5 seconds, quit
				is.close();
				break;
			}
			StringBuffer str = new StringBuffer();
			int c = 0;
			while ((c = is.read()) != -1) str.append((char) c);
			if (str.length() != 0) {
				System.out.println("got: " + str.toString());
				ack = true;

				if (str.indexOf("9") != -1) break;
			}
		}
		process.waitFor();	// wait for child process to stop

		int exitVal = process.exitValue();
		System.out.println("exit value = " + exitVal);
	}
}