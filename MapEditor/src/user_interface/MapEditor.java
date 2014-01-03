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
	private final MenuItem newTileItem = createNewTileItem();
	private final MenuItem newUnitItem = createNewUnitItem();
	private final MenuItem saveFileItem = createSaveFileItem();
	
	private Tab tileTab;
	private TabPane tabbedToolPanel;
	private ScrollPane toolPanelScroller;
	
	private Canvas canvas;
	private GraphicsContext graphicsContext;

	public static void main(String[] args) {
		launch(args);
	}

	@Override
	public void start(Stage stage) {
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

		canvas = new Canvas(2000, 2000);
		graphicsContext = canvas.getGraphicsContext2D();
		
		ScrollPane mapPane = new ScrollPane();
		mapPane.setContent(canvas);
		mapPane.setFocusTraversable(false);

		BorderPane root = new BorderPane();
		root.setTop(createMenuBar());
		root.setLeft(toolPanelScroller);
		root.setCenter(mapPane);

		Scene scene = new Scene(root, 1200, 800);
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
		
		for (UnitType type : map.getUnits()) {
			registerUnitType(type);
		}

		newTileItem.setDisable(false);
		newUnitItem.setDisable(false);
		saveFileItem.setDisable(false);

		drawMap();
		tabbedToolPanel.layout();
		tabbedToolPanel.requestLayout();
	}

	private void registerTile(Tile tile) {
		storeTileImage(tile.getName(), tile.getImageSource());

		ToggleButton tileToggle = new ToggleButton(tile.getName());
		tileToggle.setUserData(tile.getName());
		tileToggle.setToggleGroup(tileGroup);
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

	private void registerUnitType(UnitType type) {
		storeUnitImage(type.name, type.imageSource1, type.imageSource2);

		ToggleButton unitToggle = new ToggleButton(type.name);
		unitToggle.setUserData(type.name);
		unitToggle.setToggleGroup(unitGroup);
		unitPanel.getChildren().add(unitToggle);
	}

	private void storeUnitImage(String name, String source1, String source2) {
		assert (!unitImageTable.containsKey(name));

		InputStream imageStream1, imageStream2;
		Image[] images = new Image[2];
		try {
			imageStream1 = new FileInputStream(getMapDirectory() + '\\' + source1);
			images[0] = new Image(imageStream1);
			
			imageStream2 = new FileInputStream(getMapDirectory() + '\\' + source2);
			images[1] = new Image(imageStream2);
			
			unitImageTable.put(name, images);
		} catch (FileNotFoundException e) {
		}
	}

	public void addUnitType(String name, String source1, String source2,
			int health, double damage, double attackSpeed, double range,
			double speed, double collisionRadius, double selectionRadius) {
		UnitType type = new UnitType();
		type.name = name;
		type.imageSource1 = source1;
		type.imageSource2 = source2;
		type.maxHealth = health;
		type.attackDamage = damage;
		type.attackSpeed = attackSpeed;
		type.attackRange = range;
		type.speed = speed;
		type.collisionRadius = collisionRadius;
		type.selectionRadius = selectionRadius;
		map.addUnitType(type);
		registerUnitType(type);
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
		chooser.getExtensionFilters().add(
				new FileChooser.ExtensionFilter("Map Files", "*.map"));
		if (mapFile != null && mapFile.exists()) {
			chooser.setInitialDirectory(mapFile.getParentFile());
		} else {
			String pathname = getClass().getProtectionDomain().getCodeSource()
					.getLocation().getPath();
			File currentDir = new File(System.getProperty("user.dir"));
			System.out.println(currentDir.getAbsolutePath());
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
			} catch (IOException e) {
			}
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
		
		EventHandler clickHandler = new EventHandler<MouseEvent>() {
			public void handle(MouseEvent event) {
				if (map != null) {
					if (checkMapBounds(event.getX(), event.getY())) {
						if (selectedTile.length() > 0) {
							placeTile(event.getX(), event.getY());
						} else if (selectedUnit.length() > 0) {
							placeUnit(event.getX(), event.getY());
						}
					}
				}
			}
		};

		EventHandler dragHandler = new EventHandler<MouseEvent>() {
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

		canvas.setOnMouseClicked(clickHandler);
		canvas.setOnMouseDragged(dragHandler);
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
				} catch (IOException e) {
				}
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