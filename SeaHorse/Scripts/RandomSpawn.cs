using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RandomSpawn : MonoBehaviour
{
    public int numberSpawn;
    public int numberTile;
    public int maxSpawns;
    public GameObject[] gameTiles;
    public GameObject Clam;
    public Vector3 location;
    public int collectibleRandom;

    void Start()
    {
        spawnTiles();
    }

    void spawnTiles()
    {
        numberSpawn = Random.Range(1, maxSpawns);
        collectibleRandom = Random.Range(0, numberSpawn);
        for (int i = 0; i < numberSpawn; i++)
        {
            numberTile = 0;
            float x = Random.Range(-10.0f, 10.0f);
            float y = Random.Range(-3.0f, 3.0f);
            location = new Vector3(x, y, 0.0f);
            Instantiate(gameTiles[numberTile], location, Quaternion.identity);
            if (i == collectibleRandom) Instantiate(Clam, location, Quaternion.identity);
        }
        numberSpawn = Random.Range(1, 4);
        for (int i = 0; i < numberSpawn; i++)
        {
            numberTile = 1;
            float x = Random.Range(-10.0f, 10.0f);
            float y = Random.Range(-3.0f, 3.0f);
            location = new Vector3(x, y, 0.0f);
            Instantiate(gameTiles[numberTile], location, Quaternion.identity);
        }
    }
}