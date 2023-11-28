# What is this
### SBL stands for Simple Bedrock Loader
SBL is going to be the loader most if not all of my C++ mods for bedrock are made on till  [Frederox's Amethyst loader becomes better for my work flow!](https://github.com/FrederoxDev/Amethyst).
# Should I use SBL
## Long story short. No
SBL is just made as something that allows me to write minecraft bedrock mods quickly with out using third party injectors. Feel free to use it though! It will have some utilities for stuff like Vec3
# How does it work?
SBL works by
1. Moving all the mods in the mods dir to the temp dir that SBL has
2. Loading all of the mods into the games memory
3. When told too SBL then calls the init function in every mod loaded into the game
4. When told too SBL calls the destructor function for every mod loaded into the game then unloads t hem
# What does SBL have over others?
Nothing much lol. It has hot reloading (i think at the time of writing this i havent tested that or anything in the loader).
It should be compatible for mods made with Amethyst I hope! 
