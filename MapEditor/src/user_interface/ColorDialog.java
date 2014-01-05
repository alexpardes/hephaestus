package user_interface;

import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.stage.Stage;
import javafx.stage.StageStyle;

public class ColorDialog extends Stage {
	public ColorDialog() {
		super();
		setResizable(false);
		setTitle("Player Colors");
		initStyle(StageStyle.UTILITY);
		
		GridPane grid = new GridPane();
		grid.setAlignment(Pos.CENTER);
		grid.setHgap(10);
		grid.setVgap(10);
		grid.setPadding(new Insets(25, 25, 25, 25));
		
		Label player1Label = new Label("Player 1 Color");
		grid.add(player1Label, 0, 0);
		
		final ColorPicker colorPicker1 = new ColorPicker();
		colorPicker1.setValue(MapEditor.instance.getPlayerColor(0));
		grid.add(colorPicker1, 1, 0);
		
		Label player2Label = new Label("Player 2 Color");
		grid.add(player2Label, 0, 1);
		
		final ColorPicker colorPicker2 = new ColorPicker();
		colorPicker2.setValue(MapEditor.instance.getPlayerColor(1));
		grid.add(colorPicker2, 1, 1);
		
		Button okButton = new Button("OK");
		okButton.setOnAction(new EventHandler<ActionEvent>() {
			public void handle(ActionEvent e) {
				MapEditor.instance.setPlayerColor(0, colorPicker1.getValue());
				MapEditor.instance.setPlayerColor(1, colorPicker2.getValue());
				close();
			}
		});
		
		Button cancelButton = new Button("Cancel");
		cancelButton.setOnAction(new EventHandler<ActionEvent>() {
			public void handle(ActionEvent e) {
				close();
			}
		});
		
		HBox hBox = new HBox(3);
		hBox.setAlignment(Pos.BOTTOM_RIGHT);
		hBox.getChildren().addAll(okButton, cancelButton);
		grid.add(hBox, 0, 2, 2, 1);
		
		setScene(new Scene(grid));
	}
}
