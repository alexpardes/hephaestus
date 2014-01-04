package user_interface;

import core.Tile;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.control.ContextMenu;
import javafx.scene.control.MenuItem;
import javafx.scene.control.ToggleButton;

public class TileContextMenu extends ContextMenu {
	public TileContextMenu(final Tile tile) {
		super();
		MenuItem editItem = new MenuItem("Edit");
		getItems().add(editItem);
		
		editItem.setOnAction(new EventHandler<ActionEvent>() {
			public void handle(ActionEvent event) {
				MapEditor.instance.openDialog(new TileDialog(tile));
			}
		});
	}
}
