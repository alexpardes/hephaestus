package user_interface;

import javafx.beans.value.*;
import javafx.scene.control.TextField;

public class IntField extends TextField {
	public IntField() {
		super();
		init();
	}
	
	public IntField(int initialValue) {
		super(new Integer(initialValue).toString());
		init();
	}
	
	private void init() {
		textProperty().addListener(new ChangeListener<String>() {
			public void changed(ObservableValue<? extends String> o, String oldValue, String newValue) {
				if (newValue.length() != 0) {
					try {
						Integer.parseInt(newValue);
					} catch (NumberFormatException e) {
						setText(oldValue);
					}
				}
			}
		});		
	}
	
	public int getValue() {
		return Integer.parseInt(getText());
	}
}
