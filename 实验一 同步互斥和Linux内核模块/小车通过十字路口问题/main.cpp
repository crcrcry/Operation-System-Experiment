#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
using namespace std;

#define WEST 10
#define EAST 20
#define NORTH 30
#define SOUTH 40

char input[100];	//输入数据

int westFirst = 0, eastFirst = 0, northFirst = 0, southFirst = 0; //防止饥饿

pthread_mutex_t a, b, c, d;	//四个资源的锁
int empty = 4;      //路口四个资源

sem_t w_pass, e_pass, n_pass, s_pass; //四个方向通行信号
sem_t haveCar;  //路口来车信号

class queue{
public:
    pthread_t q[100]; //车辆线程的线程id
    int id[100];  //车辆的编号
    int head;   //队列头
    int rear;   //队列尾
    int count;  //队列中车辆数量
    string queue_name;	//队列名

    queue(string name){
        this->head = 0;
        this->rear = 0;
        this->count = 0;
        queue_name = name;
    }

    //汽车入队，接近路口
    void push(pthread_t car, int i){
        q[rear] = car;
        id[rear] = i;
        rear++;

        count = count + 1;
        sem_post(&haveCar);
        cout << "car" << i << " from " << queue_name << " arrives at crossing." << endl;
    }
    //汽车出队，离开路口
    void pop(){
        empty++;
        cout << "car" << id[head] << " from " << queue_name << " leaves crossing." << endl;
        head++;
    }
};

queue n("north"), s("south"), w("west"), e("east");

//中央控制线程cpu
void* cpu(void *none){
    int i;
    int whichGo;
    bool twoGo = false;

    int fourDir_whichGo = EAST;
    bool fourDir_haveCar = false;    //take turns

    cout << "CPU of crossing prepares." << endl;

    for(i = 0; i < 1000; i++){
    	//等待出现车辆的信号
        sem_wait(&haveCar);

        //防止饥饿
        if(southFirst >= 5||northFirst >= 5||eastFirst >= 5||westFirst >= 5){
            if(southFirst >= 5 && s.count > 0){
                cout << "cars in south are hungry" << endl;
                sem_post(&s_pass);
                southFirst = 0;
                continue;
            }
            if(northFirst >= 5 && n.count > 0){
                cout << "cars in north are hungry" << endl;
                sem_post(&n_pass);
                northFirst = 0;
                continue;
            }
            if(eastFirst >= 5 && e.count > 0){
                cout << "cars in east are hungry" << endl;
                sem_post(&e_pass);
                eastFirst = 0;
                continue;
            }
            if(westFirst >= 5 && w.count > 0){
                cout << "cars in west are hungry" << endl;
                sem_post(&w_pass);
                westFirst = 0;
                continue;
            }
        }

        //四个方向只有1个方向有车
        if(n.count > 0 && s.count == 0 && w.count == 0 && e.count == 0){
            whichGo = NORTH;
            n.count--;
        }else if(n.count == 0 && s.count > 0 && w.count == 0 && e.count == 0){
            whichGo = SOUTH;
            s.count--;
        }else if(n.count == 0 && s.count == 0 && w.count > 0 && e.count == 0){
            whichGo = WEST;
            w.count--;
        }else if(n.count == 0 && s.count == 0 && w.count == 0 && e.count > 0){
            whichGo = EAST;
            e.count--;
        }
        //四个方向只有2个方向有车
        else if(n.count > 0 && s.count == 0 && w.count == 0 && e.count > 0){
            whichGo = NORTH;
            n.count--;
            eastFirst++;
        }else if(n.count == 0 && s.count > 0 && w.count == 0 && e.count > 0){
            whichGo = EAST;
            e.count--;
            southFirst++;
        }else if(n.count == 0 && s.count == 0 && w.count > 0 && e.count > 0){
            whichGo = EAST;
            twoGo = true;
            w.count--;
            e.count--;
            //cout << "w e go" << endl;
        }else if(n.count > 0 && s.count == 0 && w.count > 0 && e.count == 0){
            whichGo = WEST;
            w.count--;
            northFirst++;
        }else if(n.count == 0 && s.count > 0 && w.count > 0 && e.count == 0){
            whichGo = SOUTH;
            s.count--;
            westFirst++;
        }else if(n.count > 0 && s.count > 0 && w.count == 0 && e.count == 0){
            whichGo = SOUTH;
            twoGo = true;
            n.count--;
            s.count--;
            //cout << "n s go" << endl;
        }
        //四个方向只有3个方向有车
        else if(n.count == 0 && s.count > 0 && w.count > 0 && e.count > 0){
            whichGo = EAST;
            twoGo = true;
            w.count--;
            e.count--;
            southFirst++;
            //cout << "3 go without north, south wait" << endl;
        }else if(n.count > 0 && s.count == 0 && w.count > 0 && e.count > 0){
            whichGo = EAST;
            twoGo = true;
            w.count--;
            e.count--;
            northFirst++;
            //cout << "3 go without south, north wait" << endl;
        }else if(n.count > 0 && s.count > 0 && w.count == 0 && e.count > 0){
            whichGo = SOUTH;
            twoGo = true;
            n.count--;
            s.count--;
            eastFirst++;
            //cout << "3 go without west, east wait" << endl;
        }else if(n.count > 0 && s.count > 0 && w.count > 0 && e.count == 0){
            whichGo = SOUTH;
            twoGo = true;
            n.count--;
            s.count--;
            westFirst++;
            //cout << "3 go without east, west wait" << endl;
        }
        //四个方向都有车
        else if(n.count > 0 && s.count > 0 && w.count > 0 && e.count > 0){
            fourDir_haveCar = true;
            if(fourDir_whichGo == EAST){
                fourDir_whichGo = SOUTH;
                n.count--;
                s.count--;
            }
            else{
                fourDir_whichGo = EAST;
                w.count--;
                e.count--;
            }
            //cout << "4 go" << endl;
        }

        //四个方向都有车时 情况处理
        if(fourDir_haveCar){
        	//东西通行
            if(fourDir_whichGo == EAST){
                if(empty <= 2){
                    cout << "dead lock! cars in east and west will wait 3 seconds." << endl;
                    sleep(3);
                }
                sem_post(&e_pass);
                sem_post(&w_pass);
                empty--;empty--;
                sem_wait(&haveCar);
            }
            //南北通行
            else{
                if(empty <= 2){
                    cout << "dead lock! cars in north and south will wait 3 seconds." << endl;
                    sleep(3);
                }
                sem_post(&n_pass);
                sem_post(&s_pass);
                empty--;empty--;
                sem_wait(&haveCar);
            }
            fourDir_haveCar = false;
            continue;
        }

        //来车时的处理
        switch (whichGo){
        	//东方来车（包含东西来车的情况）
            case EAST:{
                if(twoGo){
                    if(empty <= 2){
                        cout << "dead lock! cars in east and west will wait 3 seconds." << endl;
                        sleep(3);
                    }
                    sem_post(&w_pass);
                    sem_post(&e_pass);
                    empty--;empty--;
                    sem_wait(&haveCar);
                    twoGo = false;
                }else{
                    if(empty <= 1){
                        cout << "dead lock! car in east will wait 3 seconds." << endl;
                        sleep(3);
                    }
                    sem_post(&e_pass);
                    empty--;
                }
            } break;
            //南方来车（包含南北来车的情况）        	
            case SOUTH:{
                if(twoGo){
                    if(empty <= 2){
                        cout << "dead lock! cars in north and south will wait 3 seconds." << endl;
                        sleep(3);
                    }
                    sem_post(&n_pass);
                    sem_post(&s_pass);
                    empty--;empty--;
                    sem_wait(&haveCar);
                    twoGo = false;
                }else{
                    if(empty <= 1){
                        cout << "dead lock! car in south will wait 3 seconds." << endl;
                        sleep(3);
                    }
                    sem_post(&s_pass);
                    empty--;
                }
            } break;
            //西方来车（不包含东西来车的情况）
            case WEST:{
                if(empty <= 1){
                    cout << "dead lock! car in west will wait 3 seconds." << endl;
                    sleep(3);
                }
                sem_post(&w_pass);
                empty--;
            } break;
            //北方来车（不包含南北来车的情况）
            case NORTH:{
                if(empty <= 1){
                    cout << "dead lock! car in north will wait 3 seconds." << endl;
                    sleep(3);
                }
                sem_post(&n_pass);
                empty--;
            } break;
        }
        //cout << "which go: " << whichGo << endl;
        //cout << "empty: " << empty << endl;
    }
}

//小车线程处理
void* car(void* dir){
    int id;
    int i;

    //sleep(4);
    //问题：假设信号是 第二辆车占有了以后发现不是，释放，第三辆车占有了发现不是，释放，第二辆车又占有了...第一辆车死了...

    switch ((int)dir){
    	//小车来自北方
        case NORTH: {
            while(1){
                sem_wait(&n_pass);

                //判断这辆车是队列里第几辆车
                for(i = 0; pthread_self()!=n.q[i];i++);
                id = i;
            	//判断是否是队列中第一辆车
                if((id-n.head)==0){
                    break;
                }
                sem_post(&n_pass);
            }

            //通过c区域
            pthread_mutex_lock(&c);
            sleep(1);
            //即将进入d区域
            pthread_mutex_lock(&d);
            //离开c区域
            pthread_mutex_unlock(&c);
            sleep(1);
            //离开d区域
            pthread_mutex_unlock(&d);
            n.pop();

        } break;

        //小车来自南方
        case SOUTH: {
            while(1){
                sem_wait(&s_pass);

                //判断这辆车是队列里第几辆车
                for(i = 0; pthread_self()!=s.q[i];i++);
                id = i;
            	//判断是否是队列中第一辆车
                if((id-s.head)==0){
                    break;
                }
                sem_post(&s_pass);
            }

            //通过a区域
            pthread_mutex_lock(&a);
            sleep(1);
            //即将进入b区域
            pthread_mutex_lock(&b);
            //离开a区域
            pthread_mutex_unlock(&a);
            sleep(1);
            //离开b区域
            pthread_mutex_unlock(&b);
            s.pop();

        } break;

        //小车来自西方
        case WEST: {
            while(1){
                sem_wait(&w_pass);

                //判断这辆车是队列里第几辆车
                for(i = 0; pthread_self()!=w.q[i];i++);
                id = i;
            	//判断是否是队列中第一辆车
                if((id-w.head)==0){
                    break;
                }
                sem_post(&w_pass);
            }

            //逻辑关系同南北小车
            pthread_mutex_lock(&d);
            sleep(1);
            pthread_mutex_lock(&a);
            pthread_mutex_unlock(&d);
            sleep(1);
            pthread_mutex_unlock(&a);
            w.pop();

        } break;

        //小车来自东方
        case EAST: {
            while(1){
                sem_wait(&e_pass);
                
                //判断这辆车是队列里第几辆车
                for(i = 0; pthread_self()!=e.q[i];i++);
                id = i;
            	//判断是否是队列中第一辆车
                if((id-e.head)==0){
                    break;
                }
                sem_post(&e_pass);
            }

			//逻辑关系同南北小车
            pthread_mutex_lock(&b);
            sleep(1);
            pthread_mutex_lock(&c);
            pthread_mutex_unlock(&b);
            sleep(1);
            pthread_mutex_unlock(&c);
            e.pop();

        } break;
    }
}

int main(){
    pthread_t cpu_thread;

    //初始化锁和信号量
    sem_init(&w_pass,0,0);
    sem_init(&e_pass,0,0);
    sem_init(&n_pass,0,0);
    sem_init(&s_pass,0,0);
    sem_init(&haveCar,0,0);

    pthread_mutex_init(&a,NULL);
    pthread_mutex_init(&b,NULL);
    pthread_mutex_init(&c,NULL);
    pthread_mutex_init(&d,NULL);

    //创建cpu中央调度
    if(pthread_create(&cpu_thread,NULL,cpu,NULL)){
        cout << "create cpu error!" << endl;
        exit(-1);
    }

    scanf("%s", input);

    //根据输入，生成各方向小车
    for(int i = 0; i < strlen(input); i++){
        pthread_t id;
        switch (input[i]){
            case 'n':
                if(pthread_create(&id,NULL,car,(void*)NORTH)){
                    cout << "create car pthread error." << endl;
                }
                n.push(id,i);
                break;
            case 's':
                if(pthread_create(&id,NULL,car,(void*)SOUTH)){
                    cout << "create car pthread error." << endl;
                }
                s.push(id,i);
                break;
            case 'w':
                if(pthread_create(&id,NULL,car,(void*)WEST)){
                    cout << "create car pthread error." << endl;
                }
                w.push(id,i);
                break;
            case 'e':
                if(pthread_create(&id,NULL,car,(void*)EAST)){
                    cout << "create car pthread error." << endl;
                }
                e.push(id,i);
                break;
        }
    }
    //cpu结束后，主线程结束
    pthread_join(cpu_thread,NULL);
}
