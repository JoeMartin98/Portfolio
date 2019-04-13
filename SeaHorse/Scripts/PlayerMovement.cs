using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerMovement : MonoBehaviour
{
    
    public float speed;
    Rigidbody2D rb;
    private SpriteRenderer mySpriteRenderer;
    bool updateDone;
    public int unicornSwitch;

    void Start()
    {
        rb = GetComponent<Rigidbody2D>();
        mySpriteRenderer = GetComponent<SpriteRenderer>();

        int temp = PlayerPrefs.GetInt("unicornSwitch");
        if (temp == 1) unicornSwitch = 2;
        else unicornSwitch = 1;

        //players starting position
        if (PlayerPrefs.GetInt("position") == 0) transform.position = new Vector3(0.0f, 0.0f, 0.0f);
        if (PlayerPrefs.GetInt("position") == 1) transform.position = new Vector3(0.0f, 4.2f, 0.0f);
        if (PlayerPrefs.GetInt("position") == 2) transform.position = new Vector3(0.0f, -4.2f, 0.0f);
        if (PlayerPrefs.GetInt("position") == 3) transform.position = new Vector3(11.2f, 0.0f, 0.0f);
        if (PlayerPrefs.GetInt("position") == 4) transform.position = new Vector3(-11.2f, 0.0f, 0.0f);
    }

    void FixedUpdate()
    {
        float move = Input.GetAxisRaw("Horizontal");
        float moveUp = Input.GetAxisRaw("Vertical");
        rb.velocity = new Vector2(speed * move, speed * moveUp);
        if (move < 0) mySpriteRenderer.flipX = false;
        else if (move > 0) mySpriteRenderer.flipX = true;
    }
}