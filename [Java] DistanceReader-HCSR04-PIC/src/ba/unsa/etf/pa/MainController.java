package ba.unsa.etf.pa;

import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import javafx.fxml.FXML;
import javafx.scene.control.TextField;


public class MainController {

    @FXML
    private TextField centimetersText;

    private StringProperty distance = new SimpleStringProperty("55 cm");


    @FXML
    public void initialize() {
        centimetersText.setStyle("-fx-text-fill: green;");
        centimetersText.textProperty().bindBidirectional(distance);
    }

    public void updateDistance(int newDistance) {
        if (newDistance > 50)
            centimetersText.setStyle("-fx-text-fill: green;");
        else
            centimetersText.setStyle("-fx-text-fill: red;");

        distance.set(newDistance + " cm");
    }

}
