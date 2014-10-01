// Ozbirn, 09/24/13
// Reads a name and prints Hello name

import java.util.Scanner;

public class HelloYou
{
   public static void main(String args[]) throws Exception
   {
		Scanner sc = new Scanner(System.in);
		// keep listening for data from parent process
		while (true) {
		  	String counter = null;
		    if (sc.hasNext()) {
		    	// write read data back
		        counter = sc.nextLine();
		        System.out.println("counter = " + counter + "!");
		    }
		    // stop the process in the last counter
		    if (counter.indexOf('9') != -1) break;
		}
	  	sc.close();
   }
}