using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HealthSpawn : MonoBehaviour
{
    public GameObject healthPoop;
    public GameObject unicorn;
    public Health health;
    public bool dropHealth;
    public bool spawnOkay = true;
    public float smoothTime = 10.0f;
    public Vector3 unicornPosition;
    public Vector3 targetPosition;
    public int spawnNumber;
    private Vector3 smoothVelocity = Vector3.zero;
    public float randomY;
    public float randomX;

    void Start()
    {
        health = FindObjectOfType<Health>();
        dropHealth = true;
    }

    void Update()
    {
        Debug.Log(PlayerPrefs.GetInt("unicornSwitch"));
        if (health.healthScore == 1 && dropHealth && PlayerPrefs.GetInt("unicornSwitch") == 1 && spawnOkay) StartCoroutine(spawnUnicorn());
    }

    IEnumerator spawnUnicorn()
    {
        float randomNumber = Random.Range(0.0f, 100.0f);
        Debug.Log(randomNumber);
        if (randomNumber < 5.0f)
        {
            dropHealth = false;
            spawnNumber = 1;
            randomY = Random.Range(-3.0f, 3.0f);
            randomX = Random.Range(-10.0f, 10.0f);
            unicornPosition = new Vector3(-18.0f, randomY, 0.0f);
            targetPosition = new Vector3(18.0f, randomY, 0.0f);
            Instantiate(unicorn, unicornPosition, Quaternion.identity);
        }
        else 
        {
            spawnOkay = false;
            yield return new WaitForSeconds(1.0f);
            spawnOkay = true;
        }
    }
}