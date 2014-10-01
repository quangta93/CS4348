// Ozbirn, 09/24/13
// Sends name to child and reads response.

import java.io.*;
import java.lang.Runtime;

public class IO 
{
   public static void main(String args[])
   {
      try
      {            
	 int x;
	 Runtime rt = Runtime.getRuntime();

	 Process proc = rt.exec("java HelloYou");
    System.out.println("started child process");

	 OutputStream os = proc.getOutputStream();
         PrintWriter pw = new PrintWriter(os);
         System.out.println("writing ...");
	     pw.printf("Greg\n");
         // pw.flush();

    long startTime = System.currentTimeMillis();
	 InputStream is = proc.getInputStream();

    boolean responded = false;
    while (!responded) {
   	 while ((x=is.read()) != -1) {
         responded = true;
   	    System.out.println((char)x);
       }

       try {Thread.sleep(200);} catch(Exception e) {e.printStackTrace();}
     }
	     proc.waitFor();
         int exitVal = proc.exitValue();

         System.out.println("Process exited: " + exitVal);
         long endTIme = System.currentTimeMillis();
         System.out.println("time = " + (endTIme - startTime));
      } 
      catch (Throwable t)
      {
	 t.printStackTrace();
      }
   }
}

