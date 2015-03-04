package clientserverapp;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.PrintWriter;
import java.net.Socket;

public class Client implements Runnable
{
    int serverPortNumberJAVA = 55555;
    int serverPortNumberCPP = 55556;
    String fileName = "Data.txt";
    static BufferedReader fileBufferedReader;
    public static int serverChoice;
    public static boolean errorState = false;

    public void run()
    {
        try
        {
            //Initialize the server socket object by checking the user's choice
            Socket serverSocket;
            if(serverChoice == 1)
            {
                serverSocket = new Socket("localhost", serverPortNumberJAVA);
            }
            else if(serverChoice == 2)
            {
                serverSocket = new Socket("localhost", serverPortNumberCPP);
            }
            else
            {
                System.err.println("Error: Invalid option selected");
                errorState = true;
                return;
            }
            
            PrintWriter out = new PrintWriter(serverSocket.getOutputStream(), true);
            
            //Open the file
            fileBufferedReader = new BufferedReader(new FileReader(fileName));
            
            //Send the file name
            out.println(fileName);
            
            //Read the file
            String line = fileBufferedReader.readLine();
            while(line != null)
            {
                //Send the file to the server
                out.println(line);
                
                line = fileBufferedReader.readLine();
            }
            
            fileBufferedReader.close();
            serverSocket.close();
            
            System.out.println("File sent from the client");
        }
        catch(Exception e)
        {
            e.printStackTrace();
            errorState = true;
        }
    }
}
