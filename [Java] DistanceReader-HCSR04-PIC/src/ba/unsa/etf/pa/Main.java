package ba.unsa.etf.pa;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.stage.Stage;

public class Main extends Application {

    public MainController mainController;

    @Override
    public void start(Stage primaryStage) throws Exception{
        FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource("mainWindow.fxml"));
        Parent root = fxmlLoader.load();
        mainController = fxmlLoader.getController();
        primaryStage.setTitle("Serial Port Distance Reader (HC-SR04)");
        primaryStage.setScene(new Scene(root, 500, 330));
        primaryStage.setResizable(false);
        primaryStage.getIcons().add(new Image("/sensor.jpg"));

        primaryStage.setOnShown(event -> {
            new COMReader(mainController);
        });

        primaryStage.show();
    }

    public static void main(String[] args) {
        launch(args);
    }
}
