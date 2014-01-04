package user_interface;

import java.io.File;

import core.*;
import javafx.event.*;
import javafx.geometry.*;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.*;
import javafx.scene.text.*;
import javafx.stage.*;

public class MapDialog extends Stage {
	private MapEditor editor;
	
	public MapDialog() {		
		super();
		editor = MapEditor.instance;
		
		initStyle(StageStyle.UTILITY);
		setResizable(false);
		setTitle("Create New Map");		
		
		GridPane grid = new GridPane();
		grid.setAlignment(Pos.CENTER);
		grid.setHgap(10);
		grid.setVgap(10);
		grid.setPadding(new Insets(25, 25, 25, 25));
		
		Label fileLocationLabel = new Label("Choose File Location");
		grid.add(fileLocationLabel, 0, 0, 2, 1);
		
		final TextField fileLocationField = new TextField();
		grid.add(fileLocationField, 1, 1);
		
		Button fileLocationButton = new Button("Browse");
		fileLocationButton.setOnAction(new EventHandler<ActionEvent>() {
			public void handle(ActionEvent event) {
				FileChooser chooser = new FileChooser();
				chooser.setTitle("Save Map");
				chooser.getExtensionFilters().add(new FileChooser.
						ExtensionFilter("Map Files", "*.map"));
				File directory = new File(fileLocationField.getText());
				if (!directory.exists() || directory.isFile()) {
					directory = directory.getParentFile();
					if (directory == null || !directory.exists()) {
						directory = editor.getDefaultDirectory();						
					}
				}
				chooser.setInitialDirectory(directory);
				File file = chooser.showSaveDialog(new Stage());
				if (file != null) {
					String path = file.getAbsolutePath();
					if (!path.endsWith(".map")) {
						path += ".map";
					}
					fileLocationField.setText(path);
				}
			}
		});
		
		grid.add(fileLocationButton, 0, 1);
		
		Label widthLabel = new Label("Width");
		grid.add(widthLabel, 0, 2);
		
		final IntField widthField = new IntField(40);
		grid.add(widthField, 1, 2);
		
		Label heightLabel = new Label("Height");
		grid.add(heightLabel, 0, 3);
		
		final IntField heightField = new IntField(40);
		grid.add(heightField, 1, 3);
		
		Button okButton = new Button("OK");
		okButton.setOnAction(new EventHandler<ActionEvent>() {
			public void handle(ActionEvent event) {
				editor.setFileLocation(fileLocationField.getText());
				editor.setMap(new GameMap(widthField.getValue(), heightField.getValue()));
				close();
			}
		});
		
		Button cancelButton = new Button("Cancel");
		cancelButton.setOnAction(new EventHandler<ActionEvent>() {
			public void handle(ActionEvent event) {
				close();
			}
		});
		
		HBox hBox = new HBox(3);
		hBox.setAlignment(Pos.BOTTOM_RIGHT);
		hBox.getChildren().addAll(okButton, cancelButton);
		grid.add(hBox, 0, 4, 2, 1);
		
		setScene(new Scene(grid));
	}	
}
