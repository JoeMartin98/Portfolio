using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraController : MonoBehaviour
{
    public Transform target;
    public float speed;

    void Update()
    {
        transform.LookAt(target);
        transform.Translate(Vector3.right * Time.deltaTime * speed);
    }
}