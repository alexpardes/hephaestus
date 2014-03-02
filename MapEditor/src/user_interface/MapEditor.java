package user_interface;

import java.io.*;
import java.util.*;

import pathing.PathingGrid;

import com.fasterxml.jackson.core.*;
import com.fasterxml.jackson.databind.*;

import core.*;
import javafx.application.Application;
import javafx.beans.value.*;
import javafx.collections.ObservableList;
import javafx.event.*;
import javafx.geometry.Orientation;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import javafx.scene.*;
import javafx.scene.canvas.*;
import javafx.scene.control.*;
import javafx.scene.image.*;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCombination;
import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.*;
import javafx.scene.paint.*;
import javafx.scene.text.*;

public class MapEditor extends Application {
	public static MapEditor instance;
	
	static final int TILE_WIDTH = 10;
	static final int TILE_HEIGHT = 10;

	private File mapFile;
	private String selectedTile, selectedUnit;
	private final ObjectMapper jsonMapper = new ObjectMapper();
	private GameMap map;
	private final HashMap<String, Image> tileImageTable
			= new HashMap<String, Image>();
	private final HashMap<String, Image[]> unitImageTable
			= new HashMap<String, Image[]>();
	private final VBox tilePanel = new VBox();
	private final VBox unitPanel = new VBox();
	private final ToggleGroup tileGroup = new ToggleGroup();
	private final ToggleGroup unitGroup = new ToggleGroup();
	private final ComboBox<String> playerBox = new ComboBox<String>();
	
	// Menu items which should be disabled if there is no map loaded.
	private final List<MenuItem> mapDependentItems
		= new ArrayList<MenuItem>();
	
	// To do: make this a separate class.
	private Tab tileTab;
	private TabPane tabbedToolPanel;
	private ScrollPane toolPanelScroller;
	
	// To do: make this a separate class.
	private ScrollPane mapPane;
	private Canvas canvas;
	private GraphicsContext graphicsContext;
	
	private final List<GameUnit> selectedUnits = new ArrayList<GameUnit>(); 

	private Stage stage;
	
	public static void main(String[] args) {
		launch(args);
	}

	@Override
	public void start(Stage stage) {
		instance = this;
		this.stage = stage;
		
		selectedTile = "";
		selectedUnit = "";

		tilePanel.setSpacing(2);
		unitPanel.setSpacing(2);

		playerBox.getItems().addAll("Player 1", "Player 2");
		playerBox.setValue("Player 1");

		ToolBar tileBar = new ToolBar(new Label("Tiles"), new Separator(),
				tilePanel);
		tileBar.setOrientation(Orientation.VERTICAL);
		tileBar.setMaxWidth(Double.MAX_VALUE);
		
		HBox unitHBox = new HBox(20);
		unitHBox.getChildren().addAll(new Label("Units"), playerBox);
		
		ToolBar unitBar = new ToolBar(unitHBox, new Separator(),
				unitPanel);
		unitBar.setOrientation(Orientation.VERTICAL);
		unitBar.setMaxWidth(Double.MAX_VALUE);
		
		tileTab = new Tab("Tiles");
		tileTab.setContent(tileBar);
		tileTab.setClosable(false);
		Tab unitTab = new Tab("Units");
		unitTab.setClosable(false);
		unitTab.setContent(unitBar);
		tabbedToolPanel = new TabPane();
		tabbedToolPanel.getTabs().addAll(tileTab, unitTab);
		tabbedToolPanel.setMaxHeight(Double.MAX_VALUE);
		tabbedToolPanel.setPrefWidth(158);
		
		toolPanelScroller = new ScrollPane();
		toolPanelScroller.setContent(tabbedToolPanel);
		toolPanelScroller.setPrefWidth(160);
		
		mapPane = new ScrollPane();
		mapPane.setFocusTraversable(false);

		BorderPane root = new BorderPane();
		root.setTop(createMenuBar());
		root.setLeft(toolPanelScroller);
		root.setCenter(mapPane);
		root.setOnKeyPressed(keyPressedHandler);

		Scene scene = new Scene(root, 1200, 800);
		stage.setScene(scene);
		stage.setTitle("Map Editor");
		stage.show();

		attachEventHandlers();
		disableItems();
	}
	
	private void disableItems() {
		for (MenuItem item : mapDependentItems) {
			item.setDisable(true);
		}
	}
	
	private void enableItems() {
		for (MenuItem item : mapDependentItems) {
			item.setDisable(false);
		}
	}
	
	private enum Direction { LEFT, UP, RIGHT, DOWN } 
		
	private void scrollMap(Direction direction) {
		ScrollBar hScrollBar = null;
		ScrollBar vScrollBar = null;
        Set<Node> nodes = mapPane.lookupAll(".scroll-bar");
        for (final Node node : nodes) {
            if (node instanceof ScrollBar) {
                ScrollBar scrollBar = (ScrollBar) node;
                if (scrollBar.getOrientation() == Orientation.HORIZONTAL) {
                	hScrollBar = scrollBar;
                } else {
                	vScrollBar = scrollBar;
                }
            }
        }
        
        switch (direction) {
        	case  LEFT:
        		hScrollBar.decrement();
        		break;
        	case UP:
        		vScrollBar.decrement();
        		break;
        	case RIGHT:
        		hScrollBar.increment();
        		break;
        	case DOWN:
        		vScrollBar.increment();
        		break;
        }
	}
	
	private final EventHandler<KeyEvent> keyPressedHandler
			= new EventHandler<KeyEvent>() {
		public void handle(KeyEvent e) {
			switch (e.getCode()) {
				case DELETE:
					deleteSelectedUnits();
					drawMap();
					break;
				case W:
					scrollMap(Direction.UP);
					break;
				case A:
					scrollMap(Direction.LEFT);
					break;
				case S:
					scrollMap(Direction.DOWN);
					break;
				case D:
					scrollMap(Direction.RIGHT);
					break;
			}
		}
	};
	
	private final EventHandler<MouseEvent> clickHandler
			= new EventHandler<MouseEvent>() {
		public void handle(MouseEvent event) {
			if (map != null) {
				if (checkMapBounds(event.getX(), event.getY())) {
					if (selectedTile.length() > 0) {
						placeTile(event.getX(), event.getY());
					} else if (selectedUnit.length() > 0) {
						placeUnit(event.getX(), event.getY());
					} else {
						selectUnit(event.getX(), event.getY());
					}
				}
			}
		}
	};

	private final EventHandler<MouseEvent> dragHandler
			= new EventHandler<MouseEvent>() {
		public void handle(MouseEvent event) {
			if (map != null) {
				if (checkMapBounds(event.getX(), event.getY())) {
					if (selectedTile.length() > 0) {
						placeTile(event.getX(), event.getY());
					}
				}
			}
		}
	};

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
		
		for (UnitType type : map.getUnitTypes()) {
			registerUnitType(type);
		}
		
		// Not sure why either are necessary, let alone both, but the panel
		// won't resize after adding the tiles otherwise.
		tabbedToolPanel.layout();
		tabbedToolPanel.requestLayout();

		enableItems();
		
		setupCanvas();		
		drawMap();		
	}
	
	private void setupCanvas() {		
		canvas = new Canvas(map.getWidth()*TILE_WIDTH,
				map.getHeight()*TILE_HEIGHT);
		graphicsContext = canvas.getGraphicsContext2D();
		mapPane.setContent(canvas);
		canvas.setOnMouseClicked(clickHandler);
		canvas.setOnMouseDragged(dragHandler);
	}

	private void registerTile(Tile tile) {
		storeTileImage(tile);
		tilePanel.getChildren().add(new TileButton(tile, tileGroup));
	}
	
	private void reregisterTile(String oldName, Tile tile) {
		storeTileImage(tile);
		
		if (!oldName.equals(tile.getName())) {
			tileImageTable.remove(oldName);
			int position = removeTileFromPanel(oldName);
			tilePanel.getChildren().add(position,
					new TileButton(tile, tileGroup));
		}		
	}
	
	private int removeButtonFromPane(String name, Pane pane) {
		int position  = -1;
		ObservableList<Node> children = pane.getChildren();
		for (int i = 0; i < children.size(); ++i) {
			if (children.get(i).getUserData().toString().equals(name)) {
				position = i;
				break;
			}
		}
		children.remove(position);
		return position;		
	}
	
	private int removeUnitFromPanel(String name) {
		return removeButtonFromPane(name, unitPanel);
	}
	
	// Returns the tile's position in the panel.
	private int removeTileFromPanel(String name) {
		return removeButtonFromPane(name, tilePanel);
	}

	private void storeTileImage(Tile tile) {
		String tileName = tile.getName();
		String imageSource = tile.getImageSource();
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

	public void addTile(Tile tile) {
		map.addTile(tile);
		registerTile(tile);
		
		tabbedToolPanel.layout();
		tabbedToolPanel.requestLayout();
		drawMap();
	}
	
	public void editTile(String oldName, Tile newTile) {
		map.editTile(oldName, newTile);
		reregisterTile(oldName, newTile);
		drawMap();
	}
	
	public Tile getTile(String name) {
		return map.getTile(name);
	}
	
	public boolean hasTile(String tileName) { return map.hasTile(tileName); }
	public boolean hasUnitType(String typeName) {
		return map.hasUnitType(typeName);
	}

	private void registerUnitType(UnitType type) {
		storeUnitImage(type);
		unitPanel.getChildren().add(new UnitButton(type, unitGroup));
	}
	
	private void reregisterUnitType(String oldName, UnitType type) {
		storeUnitImage(type);
		if (!oldName.equals(type.name)) {
			unitImageTable.remove(oldName);
			int position = removeUnitFromPanel(oldName);
			unitPanel.getChildren().add(position,
					new UnitButton(type, unitGroup));
		}
	}
	
	private void updateUnitImages() {
		Collection<String> typeNames = unitImageTable.keySet();
		for (String typeName : typeNames) {
			storeUnitImage(map.getUnitType(typeName));
		}
		drawUnits();
	}

	private void storeUnitImage(UnitType type) {
		String name = type.name;
		String source1 = type.imageSource1;
		String source2 = type.imageSource2;

		InputStream imageStream1, imageStream2;
		Image[] images = new Image[2];
		try {
			imageStream1 = new FileInputStream(getMapDirectory() + '\\' + source1);
			Image base = new Image(imageStream1);

			imageStream2 = new FileInputStream(getMapDirectory() + '\\' + source2);
			Image mask = new Image(imageStream2);

			images[0] = ImageProcessor.createPlayerImage(base, mask, 0);
			images[1] = ImageProcessor.createPlayerImage(base, mask, 1);
			
			unitImageTable.put(name, images);
		} catch (FileNotFoundException e) { }
	}
	
	public Color getPlayerColor(int playerNum) {
		return map.getPlayerColor(playerNum);
	}
	
	public void setPlayerColor(int playerNum, Color color) {
		map.setPlayerColor(playerNum, color);
		updateUnitImages();
	}

	public void addUnitType(UnitType type) {
		map.addUnitType(type);
		registerUnitType(type);
	}
	
	public void editUnitType(String oldName, UnitType type) {
		map.editUnitType(oldName, type);
		reregisterUnitType(oldName, type);
		drawUnits();
	}

	public File getDefaultDirectory() {
		File file;
		if (mapFile != null && mapFile.exists()) {
			file = mapFile.getParentFile();
		} else {
			file = new File(System.getProperty("user.dir"));
		}

		return file;
	}

	public void openDialog(Stage dialog) {
		dialog.initOwner(stage);
		dialog.show();
	}

	private void openMapFile() {
		FileChooser chooser = new FileChooser();			
		chooser.setTitle("Open...");
		chooser.getExtensionFilters().add(
				new FileChooser.ExtensionFilter("Map Files", "*.map"));
		if (mapFile != null && mapFile.exists()) {
			chooser.setInitialDirectory(mapFile.getParentFile());
		} else {
			String pathname = getClass().getProtectionDomain().getCodeSource()
					.getLocation().getPath();
			File currentDir = new File(System.getProperty("user.dir"));
			if (currentDir.exists()) {
				chooser.setInitialDirectory(currentDir);
			}
		}
		File file = chooser.showOpenDialog(new Stage());
		if (file != null) {
			try {
				Map<String, Object> jsonMap = jsonMapper.readValue(file,
						Map.class);
				mapFile = file;
				setMap(new GameMap(jsonMap));
			} catch (IOException e) { }
		}
	}
	
	private void saveMap() {
		try {
			Map<String, Object> jsonMap = map.getJsonMap();
			jsonMap.put("pathinginfo", PathingCompiler.makePathingInfo(map));
			jsonMapper.writeValue(mapFile, jsonMap);
		} catch (IOException e) { }
	}
	
	private void saveFileAs() {
		FileChooser chooser = new FileChooser();
		chooser.setTitle("Save As");
		chooser.getExtensionFilters().add(
				new FileChooser.ExtensionFilter("Map Files", "*.map"));
		chooser.setInitialDirectory(mapFile.getParentFile());
		chooser.setInitialFileName(mapFile.getName());
		File file = chooser.showSaveDialog(new Stage());
		if (file != null) {
			mapFile = file;
			saveMap();
		}
	}

	private void drawTile(String tileName, int column, int row) {
		assert(tileImageTable.containsKey(tileName));
		Image image = tileImageTable.get(tileName);
		graphicsContext
				.drawImage(image, TILE_WIDTH * column, TILE_HEIGHT * row);
	}
	
	private void drawUnit(GameUnit unit) {		
		drawUnit(unit.typeName, unit.ownerId, unit.x, unit.y);
	}
	
	private void drawUnit(String name, int ownerId, double x, double y) {
		assert(unitImageTable.containsKey(name));
		Image[] images = unitImageTable.get(name);
		double centerX = x - images[ownerId].getWidth() / 2;
		double centerY = y - images[ownerId].getHeight() / 2;
		graphicsContext.drawImage(images[ownerId], centerX, centerY);	
	}
	
	private void drawUnits() {
		List<GameUnit> units = map.getPlacedUnits();
		for (GameUnit unit : units) {
			drawUnit(unit);
		}
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
			
			drawUnits();
		}
	}

	private void attachEventHandlers() {
		tileGroup.selectedToggleProperty().addListener(
				new ChangeListener<Toggle>() {
					public void changed(ObservableValue<? extends Toggle> ov,
							Toggle oldValue, Toggle newValue) {

						if (newValue == null) {
							selectedTile = "";
						} else {
							Object tileString = newValue.getUserData();
							selectedTile = (String) tileString;
							unitGroup.selectToggle(null);							
						}						
					}
				});
		
		unitGroup.selectedToggleProperty().addListener(
				new ChangeListener<Toggle>() {
					public void changed(ObservableValue<? extends Toggle> ov,
							Toggle oldValue, Toggle newValue) {
						
						if (newValue == null) {
							selectedUnit = "";
						} else {
							Object unitString = newValue.getUserData();
							selectedUnit = (String) unitString;
							tileGroup.selectToggle(null);
						}						
					}
				});		
	}
	
	private boolean checkMapBounds(double x, double y) {
		return x > 0 && y > 0 && x < map.getWidth() * TILE_WIDTH
				&& y < map.getHeight() * TILE_HEIGHT;
	}
	
	private void placeTile(double x, double y) {		
		int column = (int)x / TILE_WIDTH;
		int row = (int)y / TILE_HEIGHT;
		if (!map.getTile(column, row).getName()
				.equals(selectedTile)) {
			map.setTile(selectedTile, column, row);
			drawTile(selectedTile, column, row);
			drawUnits();
		}
	}

	private void placeUnit(double x, double y) {
		int playerId = 0;
		if (playerBox.getValue().equals("Player 2")) {
			playerId = 1;
		}
		map.placeUnit(selectedUnit, playerId, x, y);
		drawUnit(selectedUnit, playerId, x, y);
	}
	
	private void selectUnit(double x, double y) {
		selectedUnits.clear();
		GameUnit unit = map.getUnit(x, y);
		if (unit != null) {
			selectedUnits.add(unit);
		}
	}
	
	private void deleteSelectedUnits() {
		List<GameUnit> placedUnits = map.getPlacedUnits();
		for (GameUnit unit : selectedUnits) {
			placedUnits.remove(unit);
		}
		selectedUnits.clear();
	}
	
	/**************************************************
	 * 
	 * Menu Functions
	 * 
	 ***************************************************/

	private MenuBar createMenuBar() {
		MenuBar menuBar = new MenuBar();

		Menu viewMenu = new Menu("View");

		menuBar.getMenus().addAll(createFileMenu(), createTileMenu(),
				createUnitMenu(), createEditMenu(), viewMenu);
		return menuBar;
	}

	private Menu createFileMenu() {
		Menu fileMenu = new Menu("File");
		fileMenu.getItems().addAll(createNewFileItem(), createOpenFileItem(),
				new SeparatorMenuItem(), createSaveFileItem(), createSaveAsFileItem());

		return fileMenu;
	}

	private MenuItem createNewFileItem() {
		MenuItem item = new MenuItem("File");
		item.setOnAction(new EventHandler<ActionEvent>() {
			public void handle(ActionEvent event) {
				openDialog(new MapDialog());
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
				saveMap();
			}
		});

		mapDependentItems.add(saveFileItem);

		return saveFileItem;
	}
	
	private MenuItem createSaveAsFileItem() {
		MenuItem item = new MenuItem("Save As...");
		item.setOnAction(new EventHandler<ActionEvent>() {
			public void handle(ActionEvent e) {
				saveFileAs();				
			}
		});
		
		mapDependentItems.add(item);
		return item;
	}

	private Menu createTileMenu() {
		Menu menu = new Menu("Tiles");
		menu.getItems().add(createNewTileItem());
		return menu;
	}

	private MenuItem createNewTileItem() {
		MenuItem item = new MenuItem("New Tile");
		item.setAccelerator(KeyCombination.keyCombination("Ctrl+Alt+N"));
		item.setOnAction(new EventHandler<ActionEvent>() {
			public void handle(ActionEvent event) {
				openDialog(new TileDialog());
			}
		});
		mapDependentItems.add(item);
		return item;
	}

	private Menu createUnitMenu() {
		Menu menu = new Menu("Units");
		menu.getItems().addAll(createNewUnitItem());
		return menu;
	}

	private MenuItem createNewUnitItem() {
		MenuItem item = new MenuItem("New Unit");
		item.setAccelerator(KeyCombination.keyCombination("Ctrl+Shift+N"));
		item.setOnAction(new EventHandler<ActionEvent>() {
			public void handle(ActionEvent event) {
				openDialog(new UnitDialog());
			}
		});
		mapDependentItems.add(item);
		return item;
	}
	
	private Menu createEditMenu() {
		Menu menu = new Menu("Edit");
		
		MenuItem item = new MenuItem("Player Colors");
		item.setOnAction(new EventHandler<ActionEvent>() {
			public void handle(ActionEvent e) {
				openDialog(new ColorDialog());
			}
		});
		
		menu.getItems().add(item);
		return menu;
	}
}