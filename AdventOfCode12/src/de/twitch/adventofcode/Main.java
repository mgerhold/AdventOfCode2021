package de.twitch.adventofcode;

import java.io.IOException;

public class Main {

    public static void main(String[] args) throws IOException {
        final var caveSystem = CaveSystem.fromFile("input.txt");
        System.out.println("Number of paths: " + caveSystem.countPaths());
    }
}
