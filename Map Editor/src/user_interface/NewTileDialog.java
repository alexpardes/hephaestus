package user_interface;

import java.io.File;

import core.GameMap;
import javafx.collections.*;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.*;
import javafx.scene.*;
import javafx.scene.control.*;
import javafx.scene.layout.*;
import javafx.stage.*;

public class NewTileDialog extends Stage {
	private MapEditor editor;
	private ComboBox<String> imageSourceBox;
	
	public NewTileDialog(MapEditor mapEditor) {
		super();
		editor = mapEditor;
		
		setResizable(false);
		setTitle("Create New Tile");
		
		GridPane grid = new GridPane();
		grid.setAlignment(Pos.CENTER);
		grid.setHgap(10);
		grid.setVgap(10);
		grid.setPadding(new Insets(25, 25, 25, 25));
	
		Label nameLabel = new Label("Tile Name");
		grid.add(nameLabel, 0, 0);
		
		final TextField nameField = new TextField();
		grid.add(nameField, 1, 0);
		
		imageSourceBox = new ComboBox<String>();
		updateImageSourceBox();
		grid.add(imageSourceBox, 0, 1);
		
		final CheckBox traversableCheck = new CheckBox("Traversable");
		traversableCheck.setSelected(true);
		grid.add(traversableCheck, 0, 2);
		
		Button okButton = new Button("OK");
		okButton.setOnAction(new EventHandler<ActionEvent>() {
			public void handle(ActionEvent event) {
				editor.addTile(nameField.getText(),
								imageSourceBox.getValue(),
								traversableCheck.isSelected());
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
		grid.add(hBox, 0, 3, 2, 1);
		
		setScene(new Scene(grid));
	}
	
	private void updateImageSourceBox() {
		ObservableList<String> items = imageSourceBox.getItems();
		items.clear();
		
		File directory = editor.getMapFile().getParentFile();
		for (File file : directory.listFiles()) {
			if (!file.isDirectory()) {
				String filename = file.getName();
				int dotPos = filename.indexOf('.');
				if (dotPos > 0) {
					String extension = filename.substring(dotPos + 1).
							toLowerCase();
					if (extension.equals("png") ||
						extension.equals("jpg") ||
						extension.equals("jpeg") ||
						extension.equals("gif")) {
						
						items.add(filename);
					}
				}
			}
		}
		
		if (items.size() > 0) {
			imageSourceBox.setValue(items.get(0));
		}
	}
}
