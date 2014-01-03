package user_interface;

import javafx.beans.value.*;
import javafx.scene.control.TextField;

public class DoubleField extends TextField {
	public DoubleField() {
		super();
		init();
	}
	
	public DoubleField(int initialValue) {
		super(new Integer(initialValue).toString());
		init();
	}
	
	private void init() {
		textProperty().addListener(new ChangeListener<String>() {
			public void changed(ObservableValue<? extends String> o, String oldValue, String newValue) {
				if (newValue.length() != 0) {
					try {
						Double.parseDouble(newValue);
					} catch (NumberFormatException e) {
						setText(oldValue);
					}
				}
			}
		});		
	}
	
	public double getValue() {
		return Double.parseDouble(getText());
	}
}
