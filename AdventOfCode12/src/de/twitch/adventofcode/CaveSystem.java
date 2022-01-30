package de.twitch.adventofcode;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;
import java.util.stream.Collectors;

public class CaveSystem {
    static class Cave {
        private final String name;

        public Cave(String name) {
            assert name.length() > 0;
            this.name = name;
        }

        public boolean isBigCave() {
            return Character.isUpperCase(name.charAt(0));
        }

        public boolean isSmallCave() {
            return !isBigCave();
        }

        @Override
        public int hashCode() {
            return name.hashCode();
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj) {
                return true;
            }
            if (!(obj instanceof Cave)) {
                return false;
            }
            return name.equals(((Cave)obj).name);
        }

        @Override
        public String toString() {
            return name;
        }
    }

    static class Tunnel {
        private final Set<Cave> connectedCaves;

        public Tunnel(Cave from, Cave to) {
            connectedCaves = Set.of(from, to);
        }

        public boolean isConnectedTo(Cave cave) {
            return connectedCaves.contains(cave);
        }

        public Cave getOtherEnd(Cave from) {
            assert connectedCaves.contains(from);
            final var optional = connectedCaves.stream().filter(cave -> !cave.equals(from)).findFirst();
            assert optional.isPresent();
            return optional.get();
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;
            Tunnel tunnel = (Tunnel) o;
            return connectedCaves.equals(tunnel.connectedCaves);
        }

        @Override
        public int hashCode() {
            return Objects.hash(connectedCaves);
        }

        @Override
        public String toString() {
            final var stringBuilder = new StringBuilder();
            final var iterator = connectedCaves.iterator();
            stringBuilder.append(iterator.next());
            stringBuilder.append('-');
            stringBuilder.append(iterator.next());
            return stringBuilder.toString();
        }
    }

    private Set<Cave> caves = new HashSet<>();
    private Set<Tunnel> tunnels = new HashSet<>();

    private CaveSystem() { }

    private Cave caveByName(String name) {
        final var optional = caves.stream().filter(cave -> cave.name.equals(name)).findFirst();
        assert optional.isPresent();
        return optional.get();
    }

    public int countPaths() {
        final var startCave = caveByName("start");
        final var endCave = caveByName("end");

        final var paths = allPathsStartingFrom(startCave, List.of(startCave), endCave)
                .stream()
                .filter(path -> path.get(path.size() - 1).equals(endCave))
                .collect(Collectors.toList());
        for (final var path : paths) {
            for (final var cave : path) {
                System.out.print(cave + " -> ");
            }
            System.out.println();
        }
        return paths.size();
    }

    private List<List<Cave>> allPathsStartingFrom(Cave from, List<Cave> visited, Cave endCave) {
        final var startCave = visited.get(0);
        List<List<Cave>> result = new ArrayList<>();
        for (var tunnel : tunnels) {
            if (!tunnel.isConnectedTo(from)) {
                continue;
            }
            final var destination = tunnel.getOtherEnd(from);
            if (destination.equals(startCave)) {
                continue;
            }
            final var smallCaves = visited.stream()
                    .filter(Cave::isSmallCave)
                    .collect(Collectors.toList());
            boolean someSmallCaveHasBeenInsertedTwice = (smallCaves.stream().distinct().count() != smallCaves.size());
            if (destination.isSmallCave() && visited.contains(destination) && someSmallCaveHasBeenInsertedTwice) {
                continue;
            }
            final var copy = new ArrayList<>(visited);
            copy.add(destination);
            result.add(copy);
            if (destination.equals(endCave)) {
                continue;
            }
            final var paths = allPathsStartingFrom(destination, copy, endCave);
            result.addAll(paths);
        }
        return result;
    }

    public static CaveSystem fromFile(String filename) throws IOException {
        final var fileReader = new FileReader(filename);
        final var bufferedReader = new BufferedReader(fileReader);
        final List<String> lines = new ArrayList<>();
        String line = null;
        while ((line = bufferedReader.readLine()) != null) {
            lines.add(line);
        }
        bufferedReader.close();
        final var result = new CaveSystem();
        for (var connection : lines) {
            final var parts = connection.split("-");
            assert parts.length == 2;
            final var caves = new Cave[]{ new Cave(parts[0]), new Cave(parts[1])};
            result.caves.addAll(List.of(caves));
            result.tunnels.add(new Tunnel(caves[0], caves[1]));
        }
        return result;
    }

    @Override
    public String toString() {
        final var stringBuilder = new StringBuilder();
        for (final var tunnel : tunnels) {
            stringBuilder.append(tunnel);
            stringBuilder.append('\n');
        }
        return stringBuilder.toString();
    }
}
