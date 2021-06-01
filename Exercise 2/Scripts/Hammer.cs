using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Hammer : MonoBehaviour
{
    private int points;
    private int sign;
    public GameObject camera;
    public int timesReloaded;
    public Text infoText;
    public bool gameover;
    public int score;
    public bool active;
    // Start is called before the first frame update
    void Start()
    {
        points = 100;
        active = true;
    }

    // Update is called once per frame
    void Update()
    {
        if(active == true)
        {
            int N = camera.GetComponent<Load>().N;
            int K = camera.GetComponent<Load>().K;

            if (gameover == false)
            {
                if (Input.GetKeyDown(KeyCode.H))
                {
                    sign = 1;
                    Move();
                    SendRay();
                    sign = -1;
                    Invoke("Move", 0.2f);
                }
                if (Input.GetKeyDown(KeyCode.R))
                {
                    Reload();
                }

                if (Input.GetKeyDown(KeyCode.E))
                {
                    int L = camera.GetComponent<Load>().L;
                    if (transform.position.y > 2 * L)
                    {
                        gameover = true;
                    }
                }
                score = N * N - (int)Time.time - 50 * timesReloaded;
            }



            if (score <= 0)
            {
                score = 0;
                gameover = true;

            }
            if (gameover == true)
            {
                infoText.text = "Score: " + score + "\nHammer stamina: " + points + "\nHammers: " + K + "\nGame Over\n";
            }
            else
            {

                infoText.text = "Score: " + score + "\nHammer stamina: " + points + "\nHammers: " + K;
            }
        }
        
       

    }

    void Move()
    {
        transform.Rotate(0.0f, sign*90.0f, 0.0f, Space.Self);
    }
    void SendRay()
    {
        Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
        RaycastHit hit;

        if (Physics.Raycast(ray, out hit, 1.5f))
        {
            //Visible only on Scene Mode
            Debug.DrawLine(ray.origin, hit.point, Color.red, 2.5f);
            print("Hit Something - " + hit.transform.name);
            print("Found an object - distance: " + hit.distance);

            if (hit.transform.GetComponent<MeshRenderer>().material.color != Color.black)
            {
                if(points > 0)
                {
                    ChangeColor();
                    hit.transform.GetComponent<Cube>().hit();
                    points -= 10;
                }
                
            }

        }
    }

    void ChangeColor()
    {
        Color hammerColor = GetComponent<MeshRenderer>().material.color;
        float newRed = hammerColor.r - 0.1f;
        Debug.Log("newRed =" + newRed);
        GetComponent<MeshRenderer>().material.color =  new Color(newRed, 0f, 0f);
    }

    void Reload()
    {
        int K = camera.GetComponent<Load>().K;
        if(K > 0)
        {
            points = 100;
            GetComponent<MeshRenderer>().material.color = new Color(1f, 0f, 0f);
            camera.GetComponent<Load>().K--;
            timesReloaded++;
        }
    }
    void OnTriggerEnter(Collider other)
    {
        if (other.name == "FPSController")
        {
            Debug.Log("Camera Entered");
            Destroy(gameObject);
            camera.GetComponent<Load>().K++;
            //spotlight.SetActive(false);
        }
        
    }
}
