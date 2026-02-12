// src/modules/pathfinding.rs

use std::collections::{HashMap, HashSet, VecDeque, BinaryHeap};
use std::cmp::Reverse;
use ordered_float::NotNan;
// DebugWriter not used in this module

/// Position dans l'espace 2D
#[derive(Clone, Copy, Debug, Eq, PartialEq, Hash, PartialOrd, Ord)]
pub struct Position {
    pub x: i32,
    pub y: i32,
}

impl Position {
    pub fn new(x: i32, y: i32) -> Self {
        Position { x, y }
    }

    /// Distance de Manhattan
    pub fn manhattan_distance(&self, other: &Position) -> i32 {
        (self.x - other.x).abs() + (self.y - other.y).abs()
    }

    /// Distance euclidienne
    pub fn euclidean_distance(&self, other: &Position) -> f64 {
        let dx = (self.x - other.x) as f64;
        let dy = (self.y - other.y) as f64;
        (dx * dx + dy * dy).sqrt()
    }

    /// Voisins (4 directions)
    pub fn neighbors(&self) -> Vec<Position> {
        vec![
            Position::new(self.x + 1, self.y),
            Position::new(self.x - 1, self.y),
            Position::new(self.x, self.y + 1),
            Position::new(self.x, self.y - 1),
        ]
    }

    /// Voisins (8 directions)
    pub fn neighbors_diagonal(&self) -> Vec<Position> {
        vec![
            Position::new(self.x + 1, self.y),
            Position::new(self.x - 1, self.y),
            Position::new(self.x, self.y + 1),
            Position::new(self.x, self.y - 1),
            Position::new(self.x + 1, self.y + 1),
            Position::new(self.x + 1, self.y - 1),
            Position::new(self.x - 1, self.y + 1),
            Position::new(self.x - 1, self.y - 1),
        ]
    }
}

/// Type de terrain
#[derive(Clone, Debug, Eq, PartialEq, Hash)]
pub enum TerrainType {
    Empty,
    Wall,
    Water,
    Forest,
    Mountain,
}

/// Propriétés du terrain
#[derive(Clone, Debug)]
pub struct TerrainProperties {
    pub cost: f64,        // Coût de déplacement
    pub traversable: bool, // Peut être traversé
}

/// Carte pour le pathfinding
#[derive(Clone)]
pub struct PathfindingMap {
    width: i32,
    height: i32,
    terrain: HashMap<Position, TerrainType>,
    default_properties: HashMap<TerrainType, TerrainProperties>,
}

impl PathfindingMap {
    pub fn new(width: i32, height: i32) -> Self {
        let mut default_properties = HashMap::new();
        default_properties.insert(TerrainType::Empty, TerrainProperties { cost: 1.0, traversable: true });
        default_properties.insert(TerrainType::Wall, TerrainProperties { cost: 999.0, traversable: false });
        default_properties.insert(TerrainType::Water, TerrainProperties { cost: 3.0, traversable: true });
        default_properties.insert(TerrainType::Forest, TerrainProperties { cost: 2.0, traversable: true });
        default_properties.insert(TerrainType::Mountain, TerrainProperties { cost: 5.0, traversable: true });

        PathfindingMap {
            width,
            height,
            terrain: HashMap::new(),
            default_properties,
        }
    }

    /// Définit le terrain à une position
    pub fn set_terrain(&mut self, pos: Position, terrain: TerrainType) {
        if pos.x >= 0 && pos.x < self.width && pos.y >= 0 && pos.y < self.height {
            self.terrain.insert(pos, terrain);
        }
    }

    /// Obtient le type de terrain à une position
    pub fn get_terrain(&self, pos: &Position) -> TerrainType {
        self.terrain.get(pos).cloned().unwrap_or(TerrainType::Empty)
    }

    /// Vérifie si une position est valide et traversable
    pub fn is_valid_position(&self, pos: &Position) -> bool {
        pos.x >= 0 && pos.x < self.width && pos.y >= 0 && pos.y < self.height &&
        self.get_properties(&self.get_terrain(pos)).traversable
    }

    /// Obtient les propriétés d'un type de terrain
    pub fn get_properties(&self, terrain: &TerrainType) -> &TerrainProperties {
        self.default_properties.get(terrain).unwrap()
    }

    /// Coût de déplacement entre deux positions adjacentes
    pub fn movement_cost(&self, from: &Position, to: &Position) -> f64 {
        if !self.is_valid_position(to) {
            return f64::INFINITY;
        }

        let terrain_cost = self.get_properties(&self.get_terrain(to)).cost;

        // Bonus pour le mouvement diagonal
        let dx = (from.x - to.x).abs();
        let dy = (from.y - to.y).abs();
        let diagonal = dx > 0 && dy > 0;

        if diagonal {
            terrain_cost * 1.414 // √2
        } else {
            terrain_cost
        }
    }
}

/// Algorithme de pathfinding A*
pub struct AStarPathfinder {
    map: PathfindingMap,
}

impl AStarPathfinder {
    pub fn new(map: PathfindingMap) -> Self {
        AStarPathfinder { map }
    }

    /// Trouve le chemin le plus court entre start et goal
    pub fn find_path(&self, start: Position, goal: Position) -> Option<Vec<Position>> {
        if !self.map.is_valid_position(&start) || !self.map.is_valid_position(&goal) {
            return None;
        }

        // Priority queue entries: (f_score, Position)
        let mut open_set: BinaryHeap<Reverse<(NotNan<f64>, Position)>> = BinaryHeap::new();
        let mut came_from: HashMap<Position, Position> = HashMap::new();
        let mut g_score: HashMap<Position, f64> = HashMap::new();
        let mut f_score: HashMap<Position, NotNan<f64>> = HashMap::new();

        let start_h = NotNan::new(start.euclidean_distance(&goal)).unwrap_or(NotNan::new(0.0).unwrap());
        open_set.push(Reverse((NotNan::new(0.0).unwrap(), start)));
        g_score.insert(start, 0.0);
        f_score.insert(start, start_h);

        while let Some(Reverse((_, current))) = open_set.pop() {
            if current == goal {
                return Some(self.reconstruct_path(&came_from, current));
            }

            for neighbor in current.neighbors_diagonal() {
                let tentative_g_score = g_score.get(&current).cloned().unwrap_or(f64::INFINITY) + self.map.movement_cost(&current, &neighbor);

                if tentative_g_score < *g_score.get(&neighbor).unwrap_or(&f64::INFINITY) {
                    came_from.insert(neighbor, current);
                    g_score.insert(neighbor, tentative_g_score);
                    let f = tentative_g_score + neighbor.euclidean_distance(&goal);
                    let f_notnan = NotNan::new(f).unwrap_or(NotNan::new(f64::INFINITY).unwrap());
                    f_score.insert(neighbor, f_notnan);

                    // Push into open_set; duplicates are acceptable and filtered when popped
                    open_set.push(Reverse((f_notnan, neighbor)));
                }
            }
        }

        None // Aucun chemin trouvé
    }

    /// Reconstruit le chemin depuis came_from
    fn reconstruct_path(&self, came_from: &HashMap<Position, Position>, current: Position) -> Vec<Position> {
        let mut path = vec![current];
        let mut current = current;

        while let Some(&prev) = came_from.get(&current) {
            path.push(prev);
            current = prev;
        }

        path.reverse();
        path
    }
}

/// Pathfinding avec algorithme de Dijkstra
pub struct DijkstraPathfinder {
    map: PathfindingMap,
}

impl DijkstraPathfinder {
    pub fn new(map: PathfindingMap) -> Self {
        DijkstraPathfinder { map }
    }

    /// Trouve le chemin avec Dijkstra (pour coûts variables)
    pub fn find_path(&self, start: Position, goal: Position) -> Option<Vec<Position>> {
        if !self.map.is_valid_position(&start) || !self.map.is_valid_position(&goal) {
            return None;
        }

        let mut distances: HashMap<Position, f64> = HashMap::new();
        let mut previous: HashMap<Position, Position> = HashMap::new();
        let mut queue: BinaryHeap<Reverse<(NotNan<f64>, Position)>> = BinaryHeap::new();

        distances.insert(start, 0.0);
        queue.push(Reverse((NotNan::new(0.0).unwrap(), start)));

        while let Some(Reverse((cost_notnan, current))) = queue.pop() {
            let cost = cost_notnan.into_inner();
            if current == goal {
                return Some(self.reconstruct_path(&previous, current));
            }
            if cost > *distances.get(&current).unwrap_or(&f64::INFINITY) {
                continue;
            }

            for neighbor in current.neighbors_diagonal() {
                let new_cost = cost + self.map.movement_cost(&current, &neighbor);

                if new_cost < *distances.get(&neighbor).unwrap_or(&f64::INFINITY) {
                    distances.insert(neighbor, new_cost);
                    previous.insert(neighbor, current);
                    queue.push(Reverse((NotNan::new(new_cost).unwrap_or(NotNan::new(f64::INFINITY).unwrap()), neighbor)));
                }
            }
        }

        None
    }

    fn reconstruct_path(&self, previous: &HashMap<Position, Position>, current: Position) -> Vec<Position> {
        let mut path = vec![current];
        let mut current = current;

        while let Some(&prev) = previous.get(&current) {
            path.push(prev);
            current = prev;
        }

        path.reverse();
        path
    }
}

/// Recherche en largeur d'abord (BFS) pour chemins simples
pub struct BFSPathfinder {
    map: PathfindingMap,
}

impl BFSPathfinder {
    pub fn new(map: PathfindingMap) -> Self {
        BFSPathfinder { map }
    }

    /// Recherche BFS (plus court chemin en nombre d'étapes)
    pub fn find_path(&self, start: Position, goal: Position) -> Option<Vec<Position>> {
        if !self.map.is_valid_position(&start) || !self.map.is_valid_position(&goal) {
            return None;
        }

        let mut visited = HashSet::new();
        let mut queue = VecDeque::new();
        let mut came_from = HashMap::new();

        visited.insert(start);
        queue.push_back(start);

        while let Some(current) = queue.pop_front() {
            if current == goal {
                return Some(self.reconstruct_path(&came_from, current));
            }

            for neighbor in current.neighbors() {
                if self.map.is_valid_position(&neighbor) && !visited.contains(&neighbor) {
                    visited.insert(neighbor);
                    came_from.insert(neighbor, current);
                    queue.push_back(neighbor);
                }
            }
        }

        None
    }

    fn reconstruct_path(&self, came_from: &HashMap<Position, Position>, current: Position) -> Vec<Position> {
        let mut path = vec![current];
        let mut current = current;

        while let Some(&prev) = came_from.get(&current) {
            path.push(prev);
            current = prev;
        }

        path.reverse();
        path
    }
}

/// Gestionnaire de pathfinding multi-algorithmes
pub struct PathfindingManager {
    map: PathfindingMap,
    astar: AStarPathfinder,
    dijkstra: DijkstraPathfinder,
    bfs: BFSPathfinder,
}

impl PathfindingManager {
    pub fn new(width: i32, height: i32) -> Self {
        let map = PathfindingMap::new(width, height);
        let astar = AStarPathfinder::new(map.clone());
        let dijkstra = DijkstraPathfinder::new(map.clone());
        let bfs = BFSPathfinder::new(map.clone());

        PathfindingManager {
            map,
            astar,
            dijkstra,
            bfs,
        }
    }

    /// Trouve un chemin avec l'algorithme spécifié
    pub fn find_path(&self, algorithm: &str, start: Position, goal: Position) -> Option<Vec<Position>> {
        match algorithm {
            "astar" => self.astar.find_path(start, goal),
            "dijkstra" => self.dijkstra.find_path(start, goal),
            "bfs" => self.bfs.find_path(start, goal),
            _ => None,
        }
    }

    /// Modifie le terrain
    pub fn set_terrain(&mut self, pos: Position, terrain: TerrainType) {
        self.map.set_terrain(pos, terrain);
    }

    /// Obtient les statistiques de la carte
    pub fn get_map_stats(&self) -> MapStats {
        let mut terrain_counts = HashMap::new();

        for x in 0..self.map.width {
            for y in 0..self.map.height {
                let pos = Position::new(x, y);
                let terrain = self.map.get_terrain(&pos);
                *terrain_counts.entry(terrain).or_insert(0) += 1;
            }
        }

        MapStats {
            width: self.map.width,
            height: self.map.height,
            total_cells: (self.map.width * self.map.height) as usize,
            terrain_counts,
        }
    }
}

/// Statistiques de la carte
#[derive(Debug)]
pub struct MapStats {
    pub width: i32,
    pub height: i32,
    pub total_cells: usize,
    pub terrain_counts: HashMap<TerrainType, i32>,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_astar_pathfinding() {
        let mut map = PathfindingMap::new(10, 10);

        // Ajouter un mur
        for i in 3..7 {
            map.set_terrain(Position::new(5, i), TerrainType::Wall);
        }

        let pathfinder = AStarPathfinder::new(map);
        let start = Position::new(0, 0);
        let goal = Position::new(9, 9);

        let path = pathfinder.find_path(start, goal);
        assert!(path.is_some());

        let path = path.unwrap();
        assert!(path.len() > 1);
        assert_eq!(path[0], start);
        assert_eq!(path[path.len() - 1], goal);
    }

    #[test]
    fn test_position_neighbors() {
        let pos = Position::new(5, 5);
        let neighbors = pos.neighbors();

        assert_eq!(neighbors.len(), 4);
        assert!(neighbors.contains(&Position::new(6, 5)));
        assert!(neighbors.contains(&Position::new(4, 5)));
        assert!(neighbors.contains(&Position::new(5, 6)));
        assert!(neighbors.contains(&Position::new(5, 4)));
    }
}