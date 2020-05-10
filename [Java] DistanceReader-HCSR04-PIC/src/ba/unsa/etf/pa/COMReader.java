package ba.unsa.etf.pa;

import com.fazecast.jSerialComm.*;

import java.util.Timer;
import java.util.TimerTask;

public class COMReader {

    public static SerialPort requiredComPort;

    private StringBuilder centimeters = new StringBuilder();

    public COMReader(MainController mainController) {
        try {
            requiredComPort = SerialPort.getCommPorts()[0]; // Get first available COM port. Change according to your needs.

            requiredComPort.setBaudRate(9600);
            requiredComPort.setNumDataBits(8);
            requiredComPort.setNumStopBits(SerialPort.ONE_STOP_BIT);
            requiredComPort.setParity(SerialPort.NO_PARITY);

            requiredComPort.openPort();
            System.out.println("Serial port: " + requiredComPort.getDescriptivePortName() + " is open!");

            // Event Based Reading with jSerialComm
            requiredComPort.addDataListener(new SerialPortDataListener() {
                @Override
                public int getListeningEvents() {
                    return SerialPort.LISTENING_EVENT_DATA_AVAILABLE; // Data Available for Reading
                }

                // This callback will be triggered whenever there is any data available to be read over the serial port
                @Override
                public void serialEvent(SerialPortEvent event) {
                    if (event.getEventType() != SerialPort.LISTENING_EVENT_DATA_AVAILABLE)
                        return;

                    byte[] newData = new byte[requiredComPort.bytesAvailable()];
                    requiredComPort.readBytes(newData, newData.length);

                    // Store freshly read distance
                    centimeters.append(new String(newData));
                }
            });

            TimerTask timerTask = new TimerTask() {
                @Override
                public void run() {
                    try{
                        String[] centimetersSplit = centimeters.toString().split(",");
                        if(centimetersSplit.length > 0) {
                            mainController.updateDistance(Integer.parseInt(centimetersSplit[centimetersSplit.length - 1]));
                            centimeters = new StringBuilder();  // Empty the StringBuilder (by simply creating a new one)
                        }
                    }catch(Exception e){
                        System.out.println(e.getMessage());
                    }
                }
            };

            Timer timer = new Timer();
            timer.schedule(timerTask, 0, 350);  // Update the GUI every 300 miliseconds.

        } catch (Exception e){
            System.out.println("An error occurred: " + e.getMessage());
        }
    }
}
