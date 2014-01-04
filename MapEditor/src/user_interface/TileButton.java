package user_interface;

import core.Tile;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.ToggleGroup;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;

public class TileButton extends ToggleButton {
	public TileButton(Tile tile, ToggleGroup tileGroup) {
		super(tile.getName());
		setUserData(tile.getName());
		setToggleGroup(tileGroup);
		setContextMenu(new TileContextMenu(tile));
	}
}
