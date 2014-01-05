package user_interface;

import java.io.File;

import core.GameMap;
import core.Tile;
import javafx.collections.*;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.*;
import javafx.scene.*;
import javafx.scene.control.*;
import javafx.scene.layout.*;
import javafx.stage.*;

public class TileDialog extends Stage {
	private ComboBox<String> imageSourceBox;

	public TileDialog() {
		this(null);
	}

	public TileDialog(final Tile tile) {
		super();
		final MapEditor editor = MapEditor.instance;

		final boolean isEditing = tile != null;

		setResizable(false);
		setTitle(isEditing ? "Edit Tile" : "Create New Tile");
		initStyle(StageStyle.UTILITY);

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
				String name = nameField.getText();
				String source = imageSourceBox.getValue();
				boolean isTraversable = traversableCheck.isSelected();
				Tile newTile = new Tile(name, source, isTraversable);
				if (name.length() > 0) {
					if (!editor.hasTile(name)
							|| (isEditing && name.equals(tile.getName()))) {

						if (isEditing) {
							editor.editTile(tile.getName(), newTile);
						} else {
							editor.addTile(newTile);
						}
						close();
					}
				}
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

		if (isEditing) {
			nameField.setText(tile.getName());
			imageSourceBox.setValue(tile.getImageSource());
			traversableCheck.setSelected(tile.isTraversable());
		}

		setScene(new Scene(grid));
	}

	private void updateImageSourceBox() {
		ObservableList<String> items = imageSourceBox.getItems();
		items.clear();

		File directory = MapEditor.instance.getMapFile().getParentFile();
		for (File file : directory.listFiles()) {
			if (!file.isDirectory()) {
				String filename = file.getName();
				int dotPos = filename.lastIndexOf('.');
				if (dotPos > 0) {
					String extension = filename.substring(dotPos + 1)
							.toLowerCase();
					if (extension.equals("png") || extension.equals("jpg")
							|| extension.equals("jpeg")
							|| extension.equals("gif")) {

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
