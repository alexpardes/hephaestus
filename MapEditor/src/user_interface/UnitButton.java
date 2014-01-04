package user_interface;

import core.UnitType;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.ToggleGroup;

public class UnitButton extends ToggleButton {
	public UnitButton(UnitType type, ToggleGroup unitGroup) {
		super(type.name);
		setUserData(type.name);
		setToggleGroup(unitGroup);
		setContextMenu(new UnitContextMenu(type));
	}
}
