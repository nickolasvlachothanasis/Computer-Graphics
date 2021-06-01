using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Cube : MonoBehaviour
{
    public int points;
    public GameObject hammer;
    // Start is called before the first frame update
    void Start()
    {
        points = 3;
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void hit()
    {
        points--;
        if(points == 0)
        {
            int extra = Random.Range(0, 4);
            for(int i = 0; i < 4+extra; i++)
            {
                createMiniCube();
            }
            createHammer();
            
            Destroy(gameObject);
        }
    }

    void createMiniCube()
    {
        GameObject miniCube = Instantiate(gameObject, gameObject.transform.position, Quaternion.Euler(45, 0, 0));
        miniCube.transform.localScale = new Vector3(0.1f, 0.1f, 0.1f);
        miniCube.AddComponent<Rigidbody>();
        Destroy(miniCube, 5);
    }
    void createHammer()
    {
        float r = GetComponent<MeshRenderer>().material.color.r;
        float g = GetComponent<MeshRenderer>().material.color.g;
        float b = GetComponent<MeshRenderer>().material.color.b;
        Debug.Log("r = " + r + " g = " + g + " b = "+b);
        
        if (r == 0f && g == 0f && b == 1f)
        {
            int num = 0;
            float prob = Random.Range(0f, 1f);
            if(prob > 0.3 && prob <= 0.6)
            {
                num = 1;
            }
            else if(prob > 0.6 && prob <= 0.85)
            {
                num = 2;
            }
            else if (prob > 0.85)
            {
                num = 3;
            }
            for(int i = 0; i < num; i++)
            {

                GameObject miniHammer = Instantiate(hammer, transform.position, Quaternion.identity);

                miniHammer.GetComponent<Hammer>().active = false;
                miniHammer.AddComponent<Rigidbody>();
                miniHammer.AddComponent<BoxCollider>();
                miniHammer.AddComponent<BoxCollider>();
                miniHammer.GetComponent<BoxCollider>().isTrigger = true;
                miniHammer.GetComponent<BoxCollider>().size = new Vector3(1f, 1f, 1f);
            }



        }
    }
}
