// Ozbirn, 09/24/13
// Reads a name and prints Hello name

import java.util.Scanner;

public class HelloYou
{
   public static void main(String args[])
   {
      Scanner sc = new Scanner(System.in);
 
      String name = null;
      if (sc.hasNext())
         name = sc.nextLine(); 

      System.out.println("Hello " + name + "!");
   }

}
