#define CATCH_CONFIG_MAIN

#include <Hephaestus/Coloring.h>
#include <Hephaestus/GameState.h>
#include <Hephaestus/Util.h>
#include <Hephaestus/GameUnit.h>
#include <Hephaestus/MoveAction.h>
#include <Hephaestus/DirectedSegment.h>
#include <Hephaestus/Rectangle.h>
#include <Hephaestus/Line.h>
#include <Hephaestus/Waypoint.h>
#include <Hephaestus/SectorMap.h>
#include <Hephaestus/ShadowCaster.h>

#include <PathFinding/SubgoalPathFinder.h>
#include <PathFinding/VectorPathingGrid.h>
#include <PathFinding/GridRegion.h>
#include <PathFinding/Subgoal.h>


#include <Hephaestus/Vector2.h>
#include <cmath>
#include "catch.hpp"

bool IsApprox(double x, double y, double tolerance) {
  return std::abs(x - y) < tolerance;
}

bool IsApprox(double x, double y) {
  double tolerance = std::abs(x) / 100;
  return IsApprox(x, y, tolerance);
}

bool IsApprox(const Vector2f &a, const Vector2f &b) {
  return IsApprox(a.x, b.x) && IsApprox(a.y, b.y);
}

bool IsApprox(const Vector2f &a, CONST Vector2f &b, double tolerance) {
  return IsApprox(a.x, b.x, tolerance) && IsApprox(a.y, b.y, tolerance);
}

#define REQUIRE_EQUAL(a,b,tol) REQUIRE(IsApprox(a,b,tol))

TEST_CASE("SectorMap") {
  SectorMap sectors(Vector2f(1, 1));
  REQUIRE(sectors.Contains(Vector2f(3, 4)));

  sectors.Add(M_PI/2, M_PI, 10);
  REQUIRE(sectors.Contains(Vector2f(-2, 2)));
  REQUIRE(!sectors.Contains(Vector2f(-15, 15)));
  REQUIRE(sectors.Contains(Vector2f(50, 50)));

  REQUIRE(sectors.Contains(Vector2f(25, 30)));
  sectors.Add(M_PI/4, 3*M_PI/4, 20);
  REQUIRE(!sectors.Contains(Vector2f(25, 30)));
  REQUIRE(!sectors.Contains(Vector2f(-8, 10)));
}

TEST_CASE("SectorMap 2") {
  SectorMap sectors(Vector2f(0, 0));
  sectors.Add(M_PI/4, M_PI/2, 5);
  sectors.Add(0, M_PI, 10);

  REQUIRE(sectors.Contains(Vector2f(6, 1)));
  REQUIRE(sectors.Contains(Vector2f(-6, 1)));
  REQUIRE(!sectors.Contains(Vector2f(5, 6)));
}

TEST_CASE("SectorMap 3") {
  SectorMap sectors(Vector2f(164, 562));
  sectors.Add(5.48, 5.54, 253);
  sectors.Add(5.54, 5.59, 254);

  REQUIRE(sectors.Contains(Vector2f(1000, 500)));
}

TEST_CASE("SectorMap 4") {
  SectorMap sectors(Vector2f(0, 0));
  sectors.Add(3.05, 3.10, 11);
  sectors.Add(3.00, 3.05, 10);

  REQUIRE(sectors.Contains(Vector2f(-100, -100)));
}

TEST_CASE("SectorMap 5") {
  SectorMap sectors(Vector2f(0, 0));
  sectors.Clear();
  sectors.Add(6.14, 6.19, 1);
  sectors.Add(6.24, 0.005, 2);
  REQUIRE(sectors.Contains(Vector2f(-100, -100)));

  sectors.Clear();
  sectors.Add(6.14, 6.19, 1);
  sectors.Add(6.24, 0.005, 2);
  REQUIRE(sectors.Contains(Vector2f(-100, -100)));
}

TEST_CASE("Overwrite Sector") {
  SectorMap sectors(Vector2f(0, 0));
  sectors.Add(M_PI/4, M_PI/2, 10);
  sectors.Add(M_PI/4, M_PI/2, 20);
  REQUIRE(!sectors.Contains(Vector2f(6, 10)));
  REQUIRE(sectors.Contains(Vector2f(4, 5)));

  sectors.Add(0, M_PI, 5);
  REQUIRE(!sectors.Contains(Vector2f(4, 5)));
}

TEST_CASE("Rectangle Widest Points") {
  Rect rect(Vector2f(1, -1), Vector2f(2, 1));
  std::vector<Vector2f> widestPoints = rect.WidestPoints(Vector2f(0, 0));
  REQUIRE(widestPoints.size() == 2);
  REQUIRE_EQUAL(widestPoints[1], Vector2f(1, 1), 0.01);
  REQUIRE_EQUAL(widestPoints[0], Vector2f(1, -1), 0.01);
}

TEST_CASE("RGB-HSL Conversion - Red") {
  sf::Color rgbRed = sf::Color::Red;
  HslColor hslRed = Coloring::ConvertToHsl(rgbRed);
  REQUIRE_EQUAL(hslRed.hue, 0, 0.01);
  REQUIRE_EQUAL(hslRed.saturation, 1, 0.01);
  REQUIRE_EQUAL(hslRed.lightness, 0.5, 0.01);

  rgbRed = Coloring::ConvertToRgb(hslRed);
  REQUIRE(rgbRed.r == 255);
  REQUIRE(rgbRed.g == 0);
  REQUIRE(rgbRed.b == 0);
}

TEST_CASE("Coloring") {
  sf::Color blue = sf::Color::Blue;
  sf::Color red = sf::Color::Red;
  sf::Color newColor = Coloring::AdjustLightness(Coloring::ConvertToHsl(blue),
      Coloring::Lightness(red));
  REQUIRE(newColor.r == 0);
  REQUIRE(newColor.g == 0);
  REQUIRE(newColor.b == 255);
}

TEST_CASE("More Coloring") {
  sf::Color blue = sf::Color::Blue;
  sf::Color base(223, 107, 107, 255);
  HslColor hslBlue = Coloring::ConvertToHsl(blue);
  sf::Color result = Coloring::AdjustLightness(hslBlue, Coloring::Lightness(base));
  REQUIRE(result.r == 75);
  REQUIRE(result.g == 75);
  REQUIRE(result.b == 255);
}

TEST_CASE("Line Nearest Point") {
  Line line(Vector2f(5, 5), Vector2f(1, 0));
  Vector2f point = line.Evaluate(line.NearestPointParam(Vector2f(3, 2)));
  REQUIRE_EQUAL(point, Vector2f(3, 5), 0.01);
}

TEST_CASE("Line Segment Nearest Point") {
  LineSegment segment(Vector2f(5, 5), Vector2f(7, 5));
  Vector2f point = segment.NearestPoint(Vector2f(3, 2));
  REQUIRE_EQUAL(point, Vector2f(5, 5), 0.01);
  point = segment.NearestPoint(Vector2f(6, 7));
  REQUIRE_EQUAL(point, Vector2f(6, 5), 0.01);
}

TEST_CASE("Rectangle Nearest Point") {
  Rect rectangle(Vector2f (1, 1), Vector2f(3, 2));
  Vector2f nearest = rectangle.NearestPoint(Vector2f(2, 3));
  REQUIRE_EQUAL(nearest, Vector2f(2, 2), 0.01);
  nearest = rectangle.NearestPoint(Vector2f(10, 10));
  REQUIRE_EQUAL(nearest, Vector2f(3, 2), 0.01);
}

TEST_CASE("Waypoint") {
  Subgoal *subgoal = new Subgoal(Vector2i(5, 3), Vector2i(-1, 1));
  Waypoint waypoint(subgoal, Vector2f(50, 50));
  REQUIRE_EQUAL(waypoint.Position(0), Vector2f(250, 200), 0.01);
  REQUIRE_EQUAL(waypoint.Position(25), Vector2f(225, 225), 0.1);
  REQUIRE(waypoint.IsReached(Vector2f(224, 230), 25));
  REQUIRE(!waypoint.IsReached(Vector2f(226, 230), 25));
}

TEST_CASE("Line-Point Distance") {
  Line line(Vector2f(2, 3), Vector2f(5, 0));
  REQUIRE_EQUAL(line.Distance(Vector2f(5, 6)), 3, 0.01);
}

TEST_CASE("Circle-Segment Collision") {
  DirectedSegment path(Vector2f(3, 2), Vector2f(6, 2));
  LineSegment segment(Vector2f(5, 0), Vector2f(5, 5));
  float collisionDist = path.CollisionDistance(segment, 0.5);
  REQUIRE_EQUAL(collisionDist, 1.5, 0.01);

  path = DirectedSegment(Vector2f(3, 10), Vector2f(6, 10));
  collisionDist = path.CollisionDistance(segment, 0.5);
  REQUIRE(collisionDist > 1000);
}

TEST_CASE("Circle-Rectangle Collision") {
  Rect rectangle(Vector2f(2, 2), Vector2f(4, 4));
  DirectedSegment segment(Vector2f(0, 3), Vector2f(5, 3));
  Vector2f *point = segment.CollisionPoint(rectangle, 1);
  REQUIRE(point);
  REQUIRE_EQUAL(*point, Vector2f(1, 3), 0.1);
  segment = DirectedSegment(Vector2f(0, 6), Vector2f(3, 3));
  point = segment.CollisionPoint(rectangle, 1);
  REQUIRE(point);
  REQUIRE_EQUAL(*point, *segment.CollisionPoint(Vector2f(2, 4), 1), 0.1);
  segment = DirectedSegment(Vector2f(5, 5), Vector2f(20, 20));
  point = segment.CollisionPoint(rectangle, 1);
  REQUIRE(!point);
}

TEST_CASE("Path Finding") {
  PathingGrid *grid = new VectorPathingGrid(5, 5);
  grid->SetBlocked(0, 3);
  grid->SetBlocked(1, 1);
  grid->SetBlocked(1, 3);
  grid->SetBlocked(2, 1);
  grid->SetBlocked(2, 3);
  grid->SetBlocked(3, 0);
  grid->SetBlocked(3, 3);
  std::vector<Subgoal*> subgoals;
  subgoals.push_back(new Subgoal(Vector2i(0, 0), Vector2i(0, 0)));
  subgoals.push_back(new Subgoal(Vector2i(0, 2), Vector2i(0, 0)));
  subgoals.push_back(new Subgoal(Vector2i(3, 2), Vector2i(0, 0)));
  subgoals.push_back(new Subgoal(Vector2i(4, 1), Vector2i(0, 0)));
  subgoals.push_back(new Subgoal(Vector2i(4, 2), Vector2i(0, 0)));
  subgoals.push_back(new Subgoal(Vector2i(4, 4), Vector2i(0, 0)));
  std::vector<const std::vector<int>> adjacencyLists;
  std::vector<int> list1;
  list1.push_back(1);
  adjacencyLists.push_back(list1);
  std::vector<int> list2;
  list2.push_back(0);
  list2.push_back(2);
  adjacencyLists.push_back(list2);
  std::vector<int> list3;
  list3.push_back(1);
  list3.push_back(3);
  list3.push_back(4);
  adjacencyLists.push_back(list3);
  std::vector<int> list4;
  list4.push_back(2);
  list4.push_back(4);
  adjacencyLists.push_back(list4);
  std::vector<int> list5;
  list5.push_back(2);
  list5.push_back(3);
  list5.push_back(5);
  adjacencyLists.push_back(list5);
  std::vector<int> list6;
  list6.push_back(4);
  adjacencyLists.push_back(list6);
  SubgoalPathFinder pather(grid, Vector2f(50, 50), subgoals, adjacencyLists);
  std::vector<Waypoint*> path =
      pather.GetPath(Vector2f(125, 25), Vector2f(25, 225));
  REQUIRE(path.size() == 6);
  path = pather.GetPath(Vector2f(25, 225), Vector2f(125, 25));
  REQUIRE(path.size() == 6);
}

TEST_CASE("Empty Grid") {
  PathingGrid *grid = new VectorPathingGrid(3, 3);
  SubgoalPathFinder pather(grid, Vector2f(50, 50), std::vector<Subgoal*>(),
      std::vector<const std::vector<int>>());
  std::vector<Waypoint*> path =
      pather.GetPath(Vector2f(25, 25), Vector2f(125, 125));
  REQUIRE(path.size() == 1);
}

TEST_CASE("Grid Region") {
  GridRegion region(Vector2i(5, 5), Vector2i(1, 0), Vector2i(1, 1));
  REQUIRE(region.Contains(Vector2i(6, 6)));
  REQUIRE(!region.Contains(Vector2i(4, 4)));
}

TEST_CASE("Indirect Region") {
  GridRegion region = GridRegion::GetIndirect(Vector2i(0, 0), Vector2i(1, 2));
  REQUIRE(region.Contains(Vector2i(2, 3)));
  REQUIRE(!region.Contains(Vector2i(2, 2)));

  region = GridRegion::GetIndirect(Vector2i(0, 0), Vector2i(-1, 2));
  REQUIRE(region.Contains(Vector2i(-2, 3)));

  region = GridRegion::GetIndirect(Vector2i(0, 0), Vector2i(-1, -2));
  REQUIRE(region.Contains(Vector2i(-2, -3)));

  region = GridRegion::GetIndirect(Vector2i(0, 0), Vector2i(1, -2));
  REQUIRE(region.Contains(Vector2i(2, -3)));

  region = GridRegion::GetIndirect(Vector2i(4, 22), Vector2i(6, 19));
  REQUIRE(region.Contains(Vector2i(8, 15)));
}

//TEST_CASE("Move Unit") {
//  PathingGrid *grid = new VectorPathingGrid(5, 5);
//  grid->SetBlocked(0, 3);
//  grid->SetBlocked(1, 1);
//  grid->SetBlocked(1, 3);
//  grid->SetBlocked(2, 1);
//  grid->SetBlocked(2, 3);
//  grid->SetBlocked(3, 0);
//  grid->SetBlocked(3, 3);
//  std::vector<const Vector2i> subgoals;
//  subgoals.push_back(Vector2i(0, 0));
//  subgoals.push_back(Vector2i(0, 2));
//  subgoals.push_back(Vector2i(3, 2));
//  subgoals.push_back(Vector2i(4, 1));
//  subgoals.push_back(Vector2i(4, 2));
//  subgoals.push_back(Vector2i(4, 4));
//  std::vector<const std::vector<int>> adjacencyLists;
//  std::vector<int> list1;
//  list1.push_back(1);
//  adjacencyLists.push_back(list1);
//  std::vector<int> list2;
//  list2.push_back(0);
//  list2.push_back(1);
//  adjacencyLists.push_back(list2);
//  std::vector<int> list3;
//  list3.push_back(1);
//  list3.push_back(3);
//  list3.push_back(4);
//  adjacencyLists.push_back(list3);
//  std::vector<int> list4;
//  list4.push_back(2);
//  list4.push_back(4);
//  adjacencyLists.push_back(list4);
//  std::vector<int> list5;
//  list5.push_back(2);
//  list5.push_back(3);
//  list5.push_back(5);
//  adjacencyLists.push_back(list5);
//  std::vector<int> list6;
//  list6.push_back(4);
//  adjacencyLists.push_back(list6);
//  SubgoalPathFinder pather(grid, subgoals, adjacencyLists);
//
//  UnitDictionary unitDefinitions;
//  UnitAttributes unitAttr;
//  unitAttr.set_speed(50);
//  unitDefinitions.insert(UnitDefinition("unit a", unitAttr));
//
//  GameState state(unitDefinitions, Vector2i(5, 5), &pather);
//  REQUIRE(state.GetTile(Vector2f(1, 1)) == Vector2i(0, 0));
//  state.AddUnit("unit a", PlayerNumber::kPlayer1, Vector2f(101, 1), 0);
//  std::shared_ptr<GameUnit> unit = state.GetUnit(1);
//  REQUIRE(unit != NULL);
//  REQUIRE(state.GetTile(unit->Id()) == Vector2i(2, 0));
//
//  //MoveAbility *ability = new MoveAbility(unit, &pather, &state, 50);
//  //unit->SetAbility(ability);
//
//  MoveAction *action = new MoveAction(Vector2f(1, 201));
//  unit->SetAction(action);
//  for (int i = 0; i < 10; i++) {
//    unit->PerformAction();
//  }
//  REQUIRE(state.GetTile(unit->Id()) == Vector2i(4, 4));
//}
//
//TEST_CASE("Another Pathing Test") {
//  PathingGrid *grid = new VectorPathingGrid(5, 5);
//  grid->SetBlocked(2, 1);
//  grid->SetBlocked(2, 2);
//  grid->SetBlocked(3, 0);
//  std::vector<const Vector2i> subgoals;
//  subgoals.push_back(Vector2i(1, 0));
//  subgoals.push_back(Vector2i(1, 3));
//  subgoals.push_back(Vector2i(3, 3));
//  subgoals.push_back(Vector2i(4, 1));
//  std::vector<const std::vector<int>> adjLists;
//  std::vector<int> list0;
//  list0.push_back(1);
//  adjLists.push_back(list0);
//  std::vector<int> list1;
//  list1.push_back(0);
//  list1.push_back(2);
//  adjLists.push_back(list1);
//  std::vector<int> list2;
//  list2.push_back(1);
//  list2.push_back(3);
//  adjLists.push_back(list2);
//  std::vector<int> list3;
//  list3.push_back(2);
//  adjLists.push_back(list3);
//  SubgoalPathFinder pather(grid, subgoals, adjLists);
//
//  UnitDictionary unitDefinitions;
//  UnitAttributes unitAttr;
//  unitAttr.set_speed(50);
//  unitDefinitions.insert(UnitDefinition("unit a", unitAttr));
//
//  GameState state(unitDefinitions, Vector2i(5, 5), &pather);
//  state.AddUnit("unit a", PlayerNumber::kPlayer1, Vector2f(225, 125), 0);
//  std::shared_ptr<GameUnit> unit = state.GetUnit(1);
//  REQUIRE(unit != NULL);
//  REQUIRE(state.GetTile(unit->Id()) == Vector2i(4, 2));
//
//  UnitAction *action = new MoveAction(Vector2f(25, 225));
//  unit->SetAction(action);
//  unit->PerformAction();
//  Vector2f direction = unit->position() - Vector2f(225, 125);
//  REQUIRE_EQUAL(direction, Vector2f(-35.35, 35.35), 1);
//  REQUIRE_EQUAL(unit->Rotation(), Util::FindAngle(Vector2f(1, 0), direction), 0.1);
//}
//
//TEST_CASE("Yet Another Pathing Test") {
//  PathingGrid *grid = new VectorPathingGrid(4, 5);
//  grid->SetBlocked(1, 3);
//  grid->SetBlocked(2, 2);  
//  grid->SetBlocked(2, 3);
//  grid->SetBlocked(3, 1);
//  grid->SetBlocked(3, 3);
//  std::vector<const Vector2i> subgoals;
//  subgoals.push_back(Vector2i(0, 2));
//  subgoals.push_back(Vector2i(0, 4));
//  subgoals.push_back(Vector2i(1, 1));
//  subgoals.push_back(Vector2i(2, 0));
//  std::vector<const std::vector<int>> adjLists;
//  std::vector<int> list0;
//  list0.push_back(1);
//  list0.push_back(2);
//  adjLists.push_back(list0);
//  std::vector<int> list1;
//  list1.push_back(0);
//  adjLists.push_back(list1);
//  std::vector<int> list2;
//  list2.push_back(0);
//  list2.push_back(3);
//  adjLists.push_back(list2);
//  std::vector<int> list3;
//  list3.push_back(2);
//  adjLists.push_back(list3);
//  SubgoalPathFinder pather(grid, subgoals, adjLists);
//
//  UnitDictionary unitDefinitions;
//  UnitAttributes unitAttr;
//  unitAttr.set_speed(50);
//  unitDefinitions.insert(UnitDefinition("unit a", unitAttr));
//
//  GameState state(unitDefinitions, Vector2i(4, 5), &pather);
//  state.AddUnit("unit a", PlayerNumber::kPlayer1, Vector2f(25, 225), 0);
//  std::shared_ptr<GameUnit> unit = state.GetUnit(1);
//  REQUIRE(unit != NULL);
//  REQUIRE(state.GetTile(unit->Id()) == Vector2i(0, 4));
//
//  UnitAction *action = new MoveAction(Vector2f(125, 75));
//  unit->SetAction(action);
//  unit->PerformAction();
//  Vector2f direction = unit->position() - Vector2f(25, 225);
//  REQUIRE_EQUAL(direction, Vector2f(0, -50), 1);
//}

TEST_CASE("Circle-Circle Collision") {
  Vector2f start(3, 3);
  Vector2f end(10, 3);
  Vector2f p(7, 3);
  float radius = 2.f;
  DirectedSegment segment(start, end);
  Vector2f *collision = segment.CollisionPoint(p, radius);
  REQUIRE(collision);
  REQUIRE_EQUAL(*collision, Vector2f(5, 3), 0.1);
  delete collision;
  collision = segment.CollisionPoint(Vector2f(5, 5), 1);
  REQUIRE(!collision);
}

TEST_CASE("Angle Interpolation") {
  UnitAttributes attributes;
  GameUnit unit(1, attributes, kPlayer1, Vector2f(0, 0), M_PI / 4);
  UnitModel state1(unit);
  unit.SetRotation(7*M_PI / 4);
  UnitModel state2(unit);
  UnitModel interp(state1, state2, 0.1);
  REQUIRE_EQUAL(interp.rotation(), Util::Radians(36), 0.01);
  interp = UnitModel(state1, state2, 0.8);
  REQUIRE_EQUAL(interp.rotation(), Util::Radians(333), 0.01);
}

TEST_CASE("Shadow Casting") {
  VectorPathingGrid grid(5, 5);
  grid.SetBlocked(2, 2);
  ShadowCaster caster(grid, 10);
  auto points = caster.ShadowCast(Vector2f(5, 5),
      0, Util::Radians(85), 0);

  bool containsPoint = false;
  for (Vector2f point : points) {
    if (IsApprox(point, Vector2f(20, 20))) containsPoint = true;
  }
  REQUIRE(containsPoint);

  containsPoint = false;
  for (Vector2f point : points) {
    if (IsApprox(point, Vector2f(30, 20))) containsPoint = true;
  }
  REQUIRE(containsPoint);

  containsPoint = false;
  for (Vector2f point : points) {
    if (IsApprox(point, Vector2f(20, 30))) containsPoint = true;
  }
  REQUIRE(containsPoint);

  containsPoint = false;
  for (Vector2f point : points) {
    if (IsApprox(point, Vector2f(30, 30))) containsPoint = true;
  }
  REQUIRE(!containsPoint);
}

TEST_CASE("Shadow Casting 2") {
  VectorPathingGrid grid(10, 10);
  grid.SetBlocked(4, 4);
  ShadowCaster caster(grid, 50);
  auto points = caster.ShadowCast(Vector2f(79, 291),
    Util::Radians(280), Util::Radians(80), 0);

  bool containsPoint = false;
  for (Vector2f point : points) {
    if (IsApprox(point, Vector2f(200, 200))) containsPoint = true;
  }
  REQUIRE(containsPoint);

  containsPoint = false;
  for (Vector2f point : points) {
    if (IsApprox(point, Vector2f(200, 250))) containsPoint = true;
  }
  REQUIRE(containsPoint);

  containsPoint = false;
  for (Vector2f point : points) {
    if (IsApprox(point, Vector2f(250, 200))) containsPoint = true;
  }
  REQUIRE(!containsPoint);

  containsPoint = false;
  for (Vector2f point : points) {
    if (IsApprox(point, Vector2f(250, 250))) containsPoint = true;
  }
  REQUIRE(containsPoint);
}