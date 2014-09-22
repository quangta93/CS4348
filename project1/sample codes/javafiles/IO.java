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

	 OutputStream os = proc.getOutputStream();

         PrintWriter pw = new PrintWriter(os);
	 pw.printf("Greg\n");
         pw.flush();  

	 InputStream is = proc.getInputStream();

	 while ((x=is.read()) != -1)
	    System.out.println((char)x); 
	      
	 proc.waitFor();

         int exitVal = proc.exitValue();

         System.out.println("Process exited: " + exitVal);

      } 
      catch (Throwable t)
      {
	 t.printStackTrace();
      }
   }
}

