using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UnicornMovement : MonoBehaviour
{
    public HealthSpawn spawn;

    private Vector3 smoothVelocity = Vector3.zero;
    public float smoothTime = 1.0f;
    public bool spawned1 = false;

    void Start()
    {
        spawn = FindObjectOfType<HealthSpawn>();
    }

    void Update()
    {
        if (!spawn.dropHealth)
        {
            transform.position = Vector3.SmoothDamp(transform.position, spawn.targetPosition, ref smoothVelocity, smoothTime);

            if ((transform.position.x > spawn.randomX - 0.08f) && (transform.position.x < spawn.randomX + 0.08f) && !spawned1 && spawn.spawnNumber >= 1)
            {
                Instantiate(spawn.healthPoop, transform.position, Quaternion.identity);
                spawned1 = true;
            }
        }
    }
}