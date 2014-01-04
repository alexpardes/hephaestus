package user_interface;

import core.UnitType;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.control.ContextMenu;
import javafx.scene.control.MenuItem;

public class UnitContextMenu extends ContextMenu {
	public UnitContextMenu(final UnitType type) {
		super();
		MenuItem editItem = new MenuItem("Edit");
		getItems().add(editItem);
		
		editItem.setOnAction(new EventHandler<ActionEvent>() {
			public void handle(ActionEvent e) {
				MapEditor.instance.openDialog(new UnitDialog(type));
			}
		});
	}
}
