package clientserverapp;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;

public class Server implements Runnable
{
    int portNumber = 55555;
    static BufferedWriter fileBufferedWriter;

    public void run()
    {
        try
        {
            //Bind a server socket on port defined by the port number
            ServerSocket serverSocket = new ServerSocket(portNumber);
            
            //Receive connections from the clients
            while(true)
            {
                System.out.println("Server waiting for connections...");
                
                Socket clientSocket = serverSocket.accept();
                BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
                
                //Receive the file name from client
                String fileName = in.readLine();
                
                int extIndex = fileName.lastIndexOf('.');
                String ext = fileName.substring(extIndex);
                fileName = fileName.substring(0, extIndex);
                
                fileName += "-received" + ext;
                
                //Receive file from the client
                fileBufferedWriter = new BufferedWriter(new FileWriter(fileName));
                
                String line = in.readLine();
                while(line != null)
                {
                    //Write data to file
                    fileBufferedWriter.write(line);
                    
                    line = in.readLine();
                    
                    fileBufferedWriter.write("\r\n");
                }
                
                fileBufferedWriter.close();
                clientSocket.close();
                
                System.out.println("File received at the server");
            }
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
    }
}
