using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerAtack : MonoBehaviour
{
    public GameObject player;
    public GameObject menuButton;
    public bool isAttack = false;
    public bool canAttack = true;
    public bool takeDamage = false;
    public bool pickup = false;
    public bool isHurt = false;
    public bool pickupHealth = false;
    private bool showMenu = false;
    public bool crabDead = false;
    private CrabSpawner spawner;
    private Health health;
    private GameObject currentObject;

    void Start()
    {
        spawner = FindObjectOfType<CrabSpawner>();
        health = FindObjectOfType<Health>();
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetMouseButtonDown(0) && canAttack && !isHurt)
                StartCoroutine(attackAnim());

        if (Input.GetKeyDown(KeyCode.E) && pickup)
        {
            if (pickupHealth)
            {
                Destroy(currentObject);
                health.healthScore++; ;
                health.healthText.text = "Health : " + health.healthScore + "hp";
            }
            else
            {
                Destroy(currentObject);
                health.collectScore++;
                health.collectText.text = "Pearls Collected : " + health.collectScore;
            }
        }

        if (Input.GetKeyDown(KeyCode.Escape))
        {
            showMenu = !showMenu;
            if (showMenu)
            {
                menuButton.SetActive(true);
                Time.timeScale = 0.0f;
            }
            else
            {
                menuButton.SetActive(false);
                Time.timeScale = 1.0f;
            }
        }
    }   

    IEnumerator attackAnim()
    {
        canAttack = false;
        isAttack = true;
        player.GetComponent<Animation>().Play("attack");
        yield return new WaitForSeconds(1.0f);
        isAttack = false;
        yield return new WaitForSeconds(1.0f);
        canAttack = true;
    }

    void OnTriggerEnter2D(Collider2D other)
    {
        if (other.gameObject.tag == "Crab")
        {                   
            if (!canAttack && isAttack && !crabDead)
            {
                Destroy(other.gameObject);
                StartCoroutine(killCrab());         
            }
        }

        if (other.gameObject.tag == "Clam")
        {
            if (!canAttack && isAttack)
            {
                Instantiate(health.Pearl, other.transform.position, Quaternion.identity);
                Destroy(other.gameObject);               
            }
        }

        if (other.gameObject.tag == "Pearl")
        {
            pickup = true;
            currentObject = other.gameObject;
        }

       if(other.gameObject.tag == "Health")
       {
            pickupHealth = true;
            pickup = true;
            currentObject = other.gameObject;
       }
    }

    void OnTriggerStay2D(Collider2D other)
    {
        if (other.gameObject.tag == "Crab") if (!isAttack && !isHurt) StartCoroutine(immune());
    }

    void OnTriggerExit2D(Collider2D other)
    {
        if (other.gameObject.tag == "Pearl" || other.gameObject.tag == "Health")
        {
            pickup = false;
            pickupHealth = false;
            currentObject = null;
        }
    }

    IEnumerator immune()
    {
        health.healthScore--;
        health.healthText.text = "Health : " + health.healthScore + "hp";
        isHurt = true;
        player.GetComponent<Animation>().Play("hurt");
        yield return new WaitForSeconds(3.0f);
        isHurt = false;
    }

    IEnumerator killCrab()
    {
        spawner.currentCrabs--;      
        health.playerScore--;
        health.playerText.text = "Crabs Remaining : " + health.playerScore;
        crabDead = true;
        yield return new WaitForSeconds(1.0f);
        crabDead = false;
}
}