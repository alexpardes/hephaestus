package user_interface;

import java.io.File;

import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.CheckBox;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.stage.Stage;

public class NewUnitDialog extends Stage {
	private MapEditor editor;
	private ComboBox<String> imageSourceBox;
	
	public NewUnitDialog(MapEditor mapEditor) {
		super();
		editor = mapEditor;
		
		setResizable(false);
		setTitle("Create New Unit");
		
		GridPane grid = new GridPane();
		grid.setAlignment(Pos.CENTER);
		grid.setHgap(10);
		grid.setVgap(10);
		grid.setPadding(new Insets(25, 25, 25, 25));
		
		Label nameLabel = new Label("Unit Name");
		grid.add(nameLabel, 0, 0);
		
		final TextField nameField = new TextField();
		grid.add(nameField, 1, 0);
		
		Label imageLabel = new Label("Image");
		grid.add(imageLabel, 0, 1);

		imageSourceBox = new ComboBox<String>();
		updateImageSourceBox();
		grid.add(imageSourceBox, 1, 1);
		
		Label healthLabel = new Label("Health");
		grid.add(healthLabel, 0, 2);
		
		final DoubleField healthField = new DoubleField();
		grid.add(healthField, 1, 2);
		
		Label damageLabel = new Label("Damage");
		grid.add(damageLabel, 0, 3);
		
		final DoubleField damageField = new DoubleField();
		grid.add(damageField, 1, 3);
		
		Label attackSpeedLabel = new Label("Attack Speed");
		grid.add(attackSpeedLabel, 0, 4);
		
		final DoubleField attackSpeedField = new DoubleField();
		grid.add(attackSpeedField, 1, 4);
		
		Label rangeLabel = new Label("Range");
		grid.add(rangeLabel, 0, 5);
		
		final DoubleField rangeField = new DoubleField();
		grid.add(rangeField, 1, 5);
		
		Label speedLabel = new Label("Move Speed");
		grid.add(speedLabel, 0, 6);
		
		final DoubleField speedField = new DoubleField();
		grid.add(speedField, 1, 6);
		
		Label collisionRadiusLabel = new Label("Collision Radius");
		grid.add(collisionRadiusLabel, 0, 7);
		
		final DoubleField collisionRadiusField = new DoubleField();
		grid.add(collisionRadiusField, 1, 7);
		
		Label selectionRadiusLabel = new Label("Selection Radius");
		grid.add(selectionRadiusLabel, 0, 8);
		
		final DoubleField selectionRadiusField = new DoubleField();
		grid.add(selectionRadiusField, 1, 8);		
		
		Button okButton = new Button("OK");
		okButton.setOnAction(new EventHandler<ActionEvent>() {
			public void handle(ActionEvent event) {
				editor.addUnitType(nameField.getText(),
						imageSourceBox.getValue(), healthField.getValue(),
						damageField.getValue(), attackSpeedField.getValue(),
						rangeField.getValue(), speedField.getValue(),
						collisionRadiusField.getValue(),
						selectionRadiusField.getValue());
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
		grid.add(hBox, 0, 9, 2, 1);
		
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