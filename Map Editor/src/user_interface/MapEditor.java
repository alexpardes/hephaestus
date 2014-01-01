package user_interface;

import java.io.*;
import java.util.*;

import com.fasterxml.jackson.core.*;
import com.fasterxml.jackson.databind.*;

import core.*;
import javafx.application.Application;
import javafx.beans.value.*;
import javafx.event.*;
import javafx.geometry.Orientation;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import javafx.scene.*;
import javafx.scene.canvas.*;
import javafx.scene.control.*;
import javafx.scene.image.*;
import javafx.scene.input.KeyCombination;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.*;
import javafx.scene.paint.*;
import javafx.scene.text.*;

public class MapEditor extends Application {
	static final int TILE_WIDTH = 50;
	static final int TILE_HEIGHT = 50;

	private File mapFile;
	private String selectedTile;
	private final ObjectMapper jsonMapper = new ObjectMapper();
	private GameMap map;
	private final HashMap<String, Image> tileImageTable = new HashMap<String, Image>();
	private final ScrollPane mapPane = new ScrollPane();	
	private final VBox tilePanel = new VBox();
	private final VBox unitPanel = new VBox();
	private final ToggleGroup placementGroup = new ToggleGroup();
	private final MenuItem newTileItem = createNewTileItem();
	private final MenuItem newUnitItem = createNewUnitItem();
	private final MenuItem saveFileItem = createSaveFileItem();
	private Canvas canvas;
	private GraphicsContext graphicsContext;

	public static void main(String[] args) {
		launch(args);
	}

	@Override
	public void start(Stage stage) {
		selectedTile = "";
		
		tilePanel.setSpacing(2);
		unitPanel.setSpacing(2);

		ToolBar toolPanel = new ToolBar(tilePanel, new Separator(), unitPanel);
		toolPanel.setOrientation(Orientation.VERTICAL);
		toolPanel.setPrefWidth(100);

		canvas = new Canvas(2000, 2000);
		graphicsContext = canvas.getGraphicsContext2D();
		mapPane.setContent(canvas);
		mapPane.setFocusTraversable(false);

		BorderPane root = new BorderPane();
		root.setTop(createMenuBar());
		root.setLeft(toolPanel);
		root.setCenter(mapPane);

		Scene scene = new Scene(root, 1000, 700);
		stage.setScene(scene);
		stage.setTitle("Map Editor");
		stage.show();

		attachEventHandlers();
	}

	public File getMapFile() {
		return mapFile;
	}

	public void setFileLocation(String location) {
		assert (location.length() != 0);
		mapFile = new File(location);
		assert (mapFile.getParentFile().exists());
	}

	public String getMapDirectory() {
		return mapFile.getParent();
	}

	public void setMap(GameMap map) {
		this.map = map;
		tileImageTable.clear();
		tilePanel.getChildren().clear();
		unitPanel.getChildren().clear();

		for (Tile tile : map.getTiles()) {
			registerTile(tile);
		}

		newTileItem.setDisable(false);
		newUnitItem.setDisable(false);
		saveFileItem.setDisable(false);

		drawMap();
	}

	private void registerTile(Tile tile) {
		storeTileImage(tile.getName(), tile.getImageSource());

		ToggleButton tileToggle = new ToggleButton(tile.getName());
		tileToggle.setUserData(tile.getName());
		tileToggle.setToggleGroup(placementGroup);
		tilePanel.getChildren().add(tileToggle);
	}

	private void storeTileImage(String tileName, String imageSource) {
		assert (!tileImageTable.containsKey(tileName));

		InputStream imageStream;
		try {
			imageStream = new FileInputStream(getMapDirectory() + "\\"
					+ imageSource);
			Image image = new Image(imageStream);
			tileImageTable.put(tileName, image);
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}

	public void addTile(String name, String imageSource, boolean isTraversable) {
		assert (name.length() > 0);
		assert (imageSource.length() > 0);

		Tile tile = new Tile(name, imageSource, isTraversable);
		map.addTile(tile);
		registerTile(tile);
		drawMap();
	}
	
	public void addUnitType(String name, String source, double health,
			double damage, double attackSpeed, double range, double speed, 
			double collisionRadius, double selectionRadius) {
		UnitType type = new UnitType();
		type.name = name;
		type.imageSource = source;
		type.maxHealth = health;
		type.attackDamage = damage;
		type.attackSpeed = attackSpeed;
		type.attackRange = range;
		type.speed = speed;
		type.collisionRadius = collisionRadius;
		type.selectionRadius = selectionRadius;
		map.addUnitType(type);
	}

	private void openNewMapDialog() {
		new NewMapDialog(this).show();
	}

	private void openNewTileDialog() {
		new NewTileDialog(this).show();
	}
	
	private void openNewUnitDialog() {
		new NewUnitDialog(this).show();
	}
	
	private void openMapFile() {
		FileChooser chooser = new FileChooser();
		chooser.setTitle("Open Map");
		File file = chooser.showOpenDialog(new Stage());
		if (file != null) {
			try {
				Map<String, Object> jsonMap = jsonMapper.readValue(file, Map.class);
				mapFile = file;
				setMap(new GameMap(jsonMap));
			} catch (IOException e) { }
		}
	}

	private void drawTile(String tileName, int column, int row) {
		assert (tileImageTable.containsKey(tileName));
		Image image = tileImageTable.get(tileName);
		graphicsContext
				.drawImage(image, TILE_WIDTH * column, TILE_HEIGHT * row);
	}

	private void drawMap() {
		if (tileImageTable.size() > 0) {
			for (int column = 0; column < map.getWidth(); ++column) {
				for (int row = 0; row < map.getHeight(); ++row) {
					Tile tile = map.getTile(column, row);
					String name = tile.getName();
					drawTile(name, column, row);
				}
			}
		}
	}

	private void attachEventHandlers() {
		placementGroup.selectedToggleProperty().addListener(
				new ChangeListener<Toggle>() {
					public void changed(ObservableValue<? extends Toggle> ov,
							Toggle oldValue, Toggle newValue) {

						if (newValue == null) {
							selectedTile = "";
						} else {
							Object tileString = newValue.getUserData();
							selectedTile = (String) tileString;
						}
					}
				});

		EventHandler changeTileHandler = new EventHandler<MouseEvent>() {
			public void handle(MouseEvent event) {
				if (selectedTile.length() > 0) {
					int column = (int) event.getX() / TILE_WIDTH;
					int row = (int) event.getY() / TILE_HEIGHT;
					if (column >= 0 && column < map.getWidth()
							&& row >= 0 && row < map.getHeight()) { 
						if (!map.getTile(column, row).getName()
								.equals(selectedTile)) {
							map.setTile(selectedTile, column, row);
							drawTile(selectedTile, column, row);
						}
					}
				}
			}
		};

		canvas.setOnMouseClicked(changeTileHandler);
		canvas.setOnMouseDragged(changeTileHandler);
	}

	private MenuBar createMenuBar() {
		MenuBar menuBar = new MenuBar();

		Menu editMenu = new Menu("Edit");
		Menu viewMenu = new Menu("View");

		menuBar.getMenus().addAll(createFileMenu(), createTileMenu(),
				createUnitMenu(), editMenu, viewMenu);
		return menuBar;
	}

	private Menu createFileMenu() {
		Menu fileMenu = new Menu("File");
		fileMenu.getItems().addAll(createNewFileItem(), createOpenFileItem(),
				saveFileItem);

		return fileMenu;
	}

	private MenuItem createNewFileItem() {
		MenuItem item = new MenuItem("File");
		item.setOnAction(new EventHandler<ActionEvent>() {
			public void handle(ActionEvent event) {
				openNewMapDialog();
			}
		});

		item.setAccelerator(KeyCombination.keyCombination("Ctrl+N"));

		return item;
	}

	private MenuItem createOpenFileItem() {
		MenuItem openFileItem = new MenuItem("Open");
		openFileItem.setAccelerator(KeyCombination.keyCombination("Ctrl+O"));
		openFileItem.setOnAction(new EventHandler<ActionEvent>() {
			public void handle(ActionEvent event) {
				openMapFile();
			}
		});

		return openFileItem;
	}

	private MenuItem createSaveFileItem() {
		MenuItem saveFileItem = new MenuItem("Save");
		saveFileItem.setAccelerator(KeyCombination.keyCombination("Ctrl+S"));
		saveFileItem.setOnAction(new EventHandler<ActionEvent>() {
			public void handle(ActionEvent event) {
				try {
					jsonMapper.writeValue(mapFile, map.getJsonMap());
				} catch (IOException e) { }
			}
		});

		saveFileItem.setDisable(true);

		return saveFileItem;
	}

	private Menu createTileMenu() {
		Menu menu = new Menu("Tiles");
		menu.getItems().addAll(newTileItem);
		return menu;
	}

	private MenuItem createNewTileItem() {
		MenuItem item = new MenuItem("New Tile");
		item.setAccelerator(KeyCombination.keyCombination("Ctrl+Alt+N"));
		item.setOnAction(new EventHandler<ActionEvent>() {
			public void handle(ActionEvent event) {
				openNewTileDialog();
			}
		});
		item.setDisable(true);
		return item;
	}
	
	private Menu createUnitMenu() {
		Menu menu = new Menu("Units");
		menu.getItems().addAll(newUnitItem);
		return menu;
	}

	private MenuItem createNewUnitItem() {
		MenuItem item = new MenuItem("New Unit");
		item.setAccelerator(KeyCombination.keyCombination("Ctrl+Shift+N"));
		item.setOnAction(new EventHandler<ActionEvent>() {
			public void handle(ActionEvent event) {
				openNewUnitDialog();
			}
		});
		item.setDisable(true);		
		return item;
	}
}