package user_interface;

import core.GameMap;
import core.Tile;
import javafx.scene.canvas.Canvas;
import javafx.scene.control.Control;
import javafx.scene.control.ScrollPane;

public class MapViewer extends Control {
	private final ScrollPane scrollPane = new ScrollPane();
	private Canvas canvas;
	private MapEditor editor;
	
	public MapViewer(MapEditor editor) {
		super();
		this.editor = editor;
	}
	
	public void setSize(int x, int y) {
		canvas = new Canvas(x*MapEditor.TILE_WIDTH, y*MapEditor.TILE_HEIGHT);
		scrollPane.setContent(canvas);
	}
}