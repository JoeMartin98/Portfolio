using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CrabMovement : MonoBehaviour
{
    public GameObject player;
    public float smoothTime;    
    private Vector3 smoothVelocity = Vector3.zero;

    void Start()
    {
        player = GameObject.FindWithTag("Player");
        smoothTime = Random.Range(0.4f, 1.0f);
}

    void Update()
    {
        transform.position = Vector3.SmoothDamp(transform.position, player.transform.position, ref smoothVelocity, smoothTime);
    }
}
