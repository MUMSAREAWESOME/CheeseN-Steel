#include <SFML/Audio.hpp>
#include <SFML/Audio/Music.hpp>
#include <SFML/System/Exception.hpp>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <sys/types.h>
#include <vector>

// Source - https://stackoverflow.com/a/26084912␍
// Posted by dynamic, modified by community. See post 'Timeline' for change history␍
// Retrieved 2026-11-08, License - CC BY-SA 3.0␍

// Usage example: filePutContents("./yourfile.txt", "content", true);
void filePutContents(const std::string& name, const std::string& content, bool append = false) {
    std::ofstream outfile;
    if (append)
        outfile.open(name, std::ios_base::app);
    else
        outfile.open(name);
    outfile << content;
}

class Item{
  public:
    std::string name;
    int amount;

    Item (const std::string& Name, const int& Value){
      name = Name;
      amount = Value;
      filePutContents("./log.txt", "Created Item!\n", true);
    }
    
    ~Item(){
      filePutContents("./log.txt", "Destroyed Item!\n", true);
    }
  };

class Sword{
  public:
    int damage;
    
    Sword(const int& Damage){
      damage = Damage;
      filePutContents("./log.txt", "Created Sword!\n", true);
    }

    ~Sword(){
      filePutContents("./log.txt", "Destroyed Sword!\n", true);
    }
};

class Player{
  public:
    std::string name;
    std::vector<std::shared_ptr<Item>> inventory;
    int health = 100;
    std::shared_ptr<Sword> weapon;
    int money = 0;

    Player(const std::string& Name){
      name = Name;
      filePutContents("./log.txt", "Created Player!\n", true);
    }

    ~Player(){
      filePutContents("./log.txt", "Destroyed Player!\n", true);
    }

    void heal(){
      if (health < 100 && inventory.front()->amount > 0){
        if (inventory.front()->name == "potion"){
          health += 100 - health;
          inventory.front()->amount -= 1;
          std::cout << "Healed to full health!\n";
        }
      }
      else if (inventory.front()->amount == 0){
        std::cout << "Can't heal! Potion is empty, you must refill it after the battle!\n";
      }
      else {
        std::cout << "Can't heal! Health is already full!\n";
      }
    }
  };

class Enemy{
  public:
    std::string name;
    int health;
    int damage;
    
    Enemy(const std::string& Name, const int& Health, const int& Damage){
      name = Name;
      health = Health;
      damage = Damage;
      filePutContents("./log.txt", "Created Enemy!\n", true);
    }

    ~Enemy(){
      filePutContents("./log.txt", "Destroyed Enemy!\n", true);
    }
};

int randomRange(const int& a, const int& b){
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distr(a, b);
  return distr(gen);
}

bool attack(const std::string& target, Player& player, Enemy& enemy){
  int fail = randomRange(1, 100);
  if (target == "enemy"){
    if (fail > 25 && enemy.health > 0){
      enemy.health -= player.weapon->damage;
      std::cout << "Damaged enemy! Dealt " << player.weapon->damage << '\n';
      return true;
    }
    else if (fail <= 25){
      std::cout << "You fumbled the attack!\n";
      return false;
    }
    else{
      std::cout << "Enemy is already dead!";
      return false;
    }
  }
  else{
    if (fail > 25 && player.health > 0){
      player.health -= enemy.damage;
      std::cout << "Enemy attacked! Dealt " << enemy.damage << '\n';
      return true;
    }
    else if (fail <= 25){
      std::cout << "Enemy fumbled the attack!\n";
      return false;
    }
    else{
      std::cout << "Couldn't attack: Player is dead!\n";
      return false;
    }
  }
}

char battleMenu(){
  std::cout << "-----------------------------------------------------\n";
  std::cout << "What would you like to do? (A)ttack, (H)eal, (C)heck, (F)lee: ";
  char choice;
  std::cin >> choice;
  return choice;
  std::cout << "------------------------------------------------------\n";
}

int battle(Player& player, const bool& skipMusic){

  bool skipTheMusic = skipMusic; 

  std::shared_ptr<sf::Music> musicPtr;
  
  if (!skipTheMusic){
    try{
      std::shared_ptr<sf::Music> music = std::make_shared<sf::Music>("assets/fight1.ogg");
      musicPtr = music;
    }
    catch(sf::Exception e){
      skipTheMusic = true;
    }

    if (!skipTheMusic){
      musicPtr->setLooping(true);
      musicPtr->play();
    }
  }

  Enemy *enemy = new Enemy("Cheese Wheel", 75, 15); // Manually make an enemy
  int win; // 0 = win, 1 = loose, 2 = flee
  bool playerTurn = true;
  
  while (true){
    if (player.health <= 0){
      win = 1;
      break;
    }
    else if (enemy->health <= 0){
      win = 0;
      break;
    }
    
    if (playerTurn){
      char choice = battleMenu();
      if (choice == 'A'){
        attack("enemy", player, *enemy);
        playerTurn = false;
      }
      else if (choice == 'H'){
        player.heal();
        continue;
      }
      else if (choice == 'F'){
        std::cout << "You Fled!\n";
        win = 2;
        break;
      }
      else if (choice == 'C'){
          std::cout << "Your health: " << player.health << "\nYour heals left: " << player.inventory.front()->amount  << "\nEnemy health: " << enemy->health << "\nYour weapon's damage: " << player.weapon->damage << "\nEnemy damage: " << enemy->damage << '\n';
          continue;
      }
      else{
          std::cout << "Input not valid!\n";
          continue;
      }
    }
    else{
      attack("player", player, *enemy);
      playerTurn = true;
    }
  }

  if (!skipTheMusic) musicPtr->stop();

  delete enemy; // Free the memory manually. I didn't know where else to add manual lol.

  if (win == 0){
    return 0;
  }
  else if (win == 2){
    return 2;
  }
  else{
    return 1;
  }
}

void shop(Player& player){
  std::cout << "Welcome to the shop!\n";
  while (true){
    std::cout << "(U)pgrade Sword, (B)uy 'The Wheel Of Cheese', (Q)uit or (R)efill potion: ";
    char choice;
    std::cin >> choice;

    if (choice == 'U' && player.money >= 25){
      player.money -= 25;
      player.weapon->damage += 5;
      std::cout << "Upped damage by 5 points!\n";
      break;
    }
    else if (choice == 'R' && player.money >= 1){
      player.money -= 1;
      player.inventory.front()->amount = 3;
      std::cout << "Refilled potion!\n";
      break;
    }
    else if (choice == 'B' && player.money >= 100){
      player.money -= 100;
      std::shared_ptr<Item> cheeseWheel = std::make_shared<Item>("Cheese Wheel", 1);
      player.inventory.push_back(cheeseWheel);
      std::cout << "Bought 'The Wheel Of Cheese!'\n";
      break;
    }
    else if (choice == 'Q'){
      std::cout << "Thank you for shopping :)!\n";
      break;
    }
    else std::cout << "Invalid choice!\n";
  }
}

void backStory(const std::string& name){
  std::cout << "\nYOU: 'How the fuck did I get down here?\n";
  std::cout << "You say as you woke up.\n";
  std::cout << "You gaze arround the dungeon, every thing seems to be made out of cheese...\n";
  std::cout << "You see a tattered old sword sitting in front of you and pick it up.\n";
  std::cout << "YOU: 'It's not the best quality but it'll do...'\n";
  std::cout << "EERIE VOICE: 'To Escape " << name << " you must kill enough cheese monsters to be able to buy 'The Cheese Wheel', which costs 100$... *EVIL LAUGHTER*'\n";
  std::cout << "You feel a sudden sense of calm rush over you. You know what you must do.\n";

  std::string logo =R"(
  ___| |   | ____| ____|  ___|  ____|   \  | )   ___|__ __| ____| ____| |     
 |     |   | __|   __|  \___ \  __|      \ |/  \___ \   |   __|   __|   |
 |     ___ | |     |          | |      |\  |         |  |   |     |     |     
\____|_|  _|_____|_____|_____/ _____| _| \_|   _____/  _|  _____|_____|_____|)";

std::cout << logo << '\n' << '\n';
}

int main(){
  std::fstream file("log.txt", std::ios::out);
  if(!file.is_open()){
    std::system("touch 'log.txt'");
    std::fstream file("log.txt", std::ios::out);
    file << "Log Start:\n";
    file.close();
  }
  else {
    file << "Log Start:\n";
    file.close();
  }
  
  // Character Creation
  std::string name;
  std::cout << "Enter name for character: ";
  std::getline(std::cin, name);
  std::unique_ptr<Player> player = std::make_unique<Player>(name);

  // Setup of player with standard items
  std::shared_ptr<Item> potion = std::make_shared<Item>("potion", 3);
  player->inventory.push_back(potion);
  std::shared_ptr<Sword> sword = std::make_shared<Sword>(10);
  player->weapon = sword;

  // Setup Game Loop
  std::string bgMusicFile = "assets/Mist.ogg";
  bool quit = false;
  char choice;
  bool skipMusic = false;
  
  // Background music    
  
  std::shared_ptr<sf::Music> musicPtr;

  try{
    std::shared_ptr<sf::Music> music = std::make_shared<sf::Music>(bgMusicFile);
    musicPtr = music;
  }
  catch(sf::Exception e){
    skipMusic = true;
  }

  if (!skipMusic){
    musicPtr->setLooping(true);
    musicPtr->play();
  }
  
  // Back story
  backStory(player->name);

  

  // Game loop
  while (true){

    if (player->inventory.back()->name == "Cheese Wheel") std::cout << "What action? (B)attle, (C)heck, (H)eal, (S)hop, (Q)uit to win: ";
    else std::cout << "What action? (B)attle, (C)heck, (H)eal, (S)hop, (Q)uit: ";
    
    std::cin >> choice;
    if (choice == 'B'){
      musicPtr->pause();
      int bWin = battle(*player, skipMusic);
      if (bWin == 0){
        if (!skipMusic) musicPtr->play();
        std::cout << "You won the battle!\n";
        player->money += 2;
      }
      else if (bWin == 2){
        if (!skipMusic) musicPtr->play();
        continue;
      }
      else{
        std::cout << "You lost the battle!\n";
        break;
      }
    }
    else if (choice == 'C'){
      std::cout << "Health: " << player->health << "\nYour heals left: " << player->inventory.front()->amount;
      std::cout << '\n';
    }
    else if (choice == 'H'){
      player->heal();
    }
    else if (choice == 'Q'){
      quit = true;
      break;
    }
    else if (choice == 'S'){
      shop(*player);
    }
    else{
      std::cout << "Please enter a valid choice!\n";
    }
  }

  if (!skipMusic){
    musicPtr->stop();
  }
  
  if (player->inventory.back()->name == "Cheese Wheel") {
    std::cout << "You Win!\n" << "Good job: " << player->name;
    
    try{
      std::shared_ptr<sf::Music> music = std::make_shared<sf::Music>("win.mp3");
      musicPtr = music;
    }
    catch(sf::Exception e){
      skipMusic = true;
    }

    if (!skipMusic){
      musicPtr->play();
    }

    while (true){
      std::cout << "(Q)uit?: ";
      char choice;
      std::cin >> choice;

      if (choice == 'Q') break;
      else {
        std::cout << "Invalid input!\n";
        continue;
      }
    }  
  }
  else if (quit) std::cout << "Hey! Thats cheating! Leaving before you finnish the game... What a fag\n";
  else std::cout << "You Loose! You must be a really stupid person to loose to this game! HAHAHA\n";
  
  return 0;
}
