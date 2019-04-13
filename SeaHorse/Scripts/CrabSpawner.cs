using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CrabSpawner : MonoBehaviour
{
    public GameObject[] Spawners;
    public GameObject Crab;
    public int currentCrabs = 0;
    public int maxCrabs;
    private float waitTime;

    public int spawnRoom;
    private bool ready = true;

    private Health health;

    void Start()
    {
        health = FindObjectOfType<Health>();
    }

    void Update()
    {
        if (currentCrabs < maxCrabs && ready && health.crabsLeft) StartCoroutine(SpawnCrab());
    }

    IEnumerator SpawnCrab()
    {
        waitTime = Random.Range(0.1f, 5.0f);
        ready = false;
        currentCrabs++;
        spawnRoom = Random.Range(0, 4);
        Instantiate(Crab, Spawners[spawnRoom].transform.position, Quaternion.identity);
        yield return new WaitForSeconds(waitTime);
        ready = true;
    }
}