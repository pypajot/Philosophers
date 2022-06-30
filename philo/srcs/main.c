/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ppajot <ppajot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/25 20:24:04 by ppajot            #+#    #+#             */
/*   Updated: 2022/06/30 20:36:31 by ppajot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../philo.h"

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

int	ft_strcmp(char *str1, char *str2)
{
	while (*str1 && *str1 == *str2)
	{
		str1++;
		str2++;
	}
	return (*str1 - *str2);
}

int	display(long t_zero, int id, t_philo *philo, char *str)
{
	struct timeval	tv;
	int				i;

	i = ft_strcmp(str, "died\n");
	pthread_mutex_lock(philo->display);
	if (!(*philo->end))
		return (pthread_mutex_unlock(philo->display), 0);
	gettimeofday(&tv, 0);
	printf("%li %i %s", tv.tv_sec * 1000 + tv.tv_usec / 1000 - t_zero,
		id, str);
	if (i == 0)
		*philo->end = 0;
	pthread_mutex_unlock(philo->display);
	return (1);
}

int	takes_fork(t_philo *philo, int id, int forknbr)
{
	if (forknbr == 2)
		pthread_mutex_lock(philo->fork2);
	else
		pthread_mutex_lock(philo->fork1);
	if (!display(*philo->t_zero, id, philo, "has taken a fork\n"))
	{
		if (forknbr == 2)
		{
			pthread_mutex_unlock(philo->fork2);
			pthread_mutex_unlock(philo->fork1);
		}
		else
			pthread_mutex_unlock(philo->fork1);
		return (0);
	}
	return (1);
}

/*int	check_fork(t_data *data, int id, int forknbr)
{
	int	res;
	int	fork;

	if (forknbr == 2)
		fork = ((id / 2) * 2) % data->philnbr;
	else
		fork = (((id - 1) / 2) * 2 + 1) % data->philnbr;
	pthread_mutex_lock(&data->forkmutex[fork]);
	if (data->forktab[fork])
		res = 1;
	else
		res = 0;
	pthread_mutex_unlock(&data->forkmutex[fork]);
	return (res);
}
*/
int	check_last_eat(t_philo *philo)
{
	int				res;
	struct timeval	tv;

	pthread_mutex_lock(&philo->lasteatmutex);
	gettimeofday(&tv, 0);
	if (tv.tv_sec * 1000 + tv.tv_usec / 1000 - philo->lasteat > philo->timetodie)
		res = 1;
	else
		res = 0;
	pthread_mutex_unlock(&philo->lasteatmutex);
	return (res);
}

int	think(t_philo *philo, int id)
{
	if (!display(*philo->t_zero, id, philo, "is thinking\n"))
		return (0);
	return (1);
}

int	ft_sleep(t_philo *philo, int id)
{
	if (!display(*philo->t_zero, id, philo, "is sleeping\n"))
		return (0);
	usleep(philo->timetosleep * 1000);
	return (1);
}

int	eat(t_philo *philo, int id)
{
	struct timeval	tv;
	int res;

	res = 1;
	pthread_mutex_lock(&philo->lasteatmutex);
	gettimeofday(&tv, 0);
	philo->lasteat = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	pthread_mutex_unlock(&philo->lasteatmutex);
	if (!display(*philo->t_zero, id, philo, "is eating\n"))
		res = 0;
	pthread_mutex_lock(&philo->meals);
	philo->mealcount++;
	pthread_mutex_unlock(&philo->meals);
	usleep(philo->timetoeat * 1000);
	pthread_mutex_unlock(philo->fork2);
	pthread_mutex_unlock(philo->fork1);
	return (res);
}

void	*philo_live(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	pthread_mutex_lock(philo->start);
	pthread_mutex_unlock(philo->start);
	if (!(philo->id % 2))
		usleep((philo->timetoeat - 1) * 1000);
	else
		usleep(philo->id * 2);
	while (1)
	{
		if (!takes_fork(philo, philo->id, 1))
			return (0);
		if (!takes_fork(philo, philo->id, 2))
			return (0);
		if(!eat(philo, philo->id))
			return (0);
		if (!ft_sleep(philo, philo->id))
			return (0);
		if (!think(philo, philo->id))
			return (0);
	}
	return (0);
}

void	zero_end(t_data *data)
{
	pthread_mutex_lock(&data->display);
	data->end = 0;
	pthread_mutex_unlock(&data->display);
}

int	ender(t_data *data, t_philo *philo)
{
	int		i;
	int		counter;

	usleep(data->timetodie * 250);
	while (1)
	{
		i = -1;
		counter = 0;
		while (++i < data->philnbr)
		{
			pthread_mutex_lock(&philo[i].meals);
			if (data->mealcount[i] >= data->mealnbr)
				counter++;
			pthread_mutex_unlock(&philo[i].meals);
			if (counter == data->philnbr)
				return (zero_end(data), 0);
			if (check_last_eat(&philo[i]))
			{
				display(data->t_zero, i + 1, &philo[i], "died\n");
				return (0);
			}
		}
	}
	return (0);
}

int	ft_atoi(char *str)
{
	int	res;

	res = 0;
	while (*str)
	{
		res = *str - 48 + res * 10;
		str++;
	}
	return (res);
}

int	check_arg(char *str)
{
	int	i;

	i = -1;
	while (str[++i])
		if (str[i] < '0' || str[i] > '9')
			return (0);
	return (1);
}

int	check_av(int ac, char **av)
{
	int	i;

	i = 0;
	if (ac < 5 || ac > 6)
		return (0);
	while (++i < ac)
	{
		if (!check_arg(av[i]))
			return (0);
		if (i == 1 && !ft_atoi(av[i]))
			return (0);
	}
	return (1);
}

void	free_data(t_data *data)
{
	int	i;

	i = -1;
	pthread_mutex_destroy(&data->display);	
	while (++i < data->philnbr)
	{
		pthread_mutex_destroy(&data->philo[i].lasteatmutex);
		pthread_mutex_destroy(&data->philo[i].meals);
		pthread_mutex_destroy(&data->forkmutex[i]);
	}
	free(data->philo);
	free(data->mealcount);
	free(data->tid);
	free(data->forkmutex);
}

int	alloc_data(t_data *data)
{
	data->philo = (t_philo *)malloc(sizeof(t_philo) * data->philnbr);
	data->mealcount = (int *)malloc(sizeof(int) * data->philnbr);
	data->tid = (pthread_t *)malloc(sizeof(pthread_t) * data->philnbr);
	data->forkmutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)
			* data->philnbr);
	if (!data->mealcount || !data->tid || !data->forkmutex)
		return (free_data(data), 0);
	return (1);
}

void	init_base_data(int ac, char **av, t_data *data)
{
	struct timeval	tv;

	gettimeofday(&tv, 0);
	data->t_zero = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	data->end = 1;
	data->philnbr = ft_atoi(av[1]);
	data->timetodie = ft_atoi(av[2]);
	data->timetoeat = ft_atoi(av[3]);
	data->timetosleep = ft_atoi(av[4]);
	if (ac == 6)
		data->mealnbr = ft_atoi(av[5]);
	else
		data->mealnbr = 2147483647;
}

void	init_philo(t_data *data, t_philo *philo, int i)
{
	philo->id = i + 1;
	philo->lasteat = data->t_zero;
	philo->t_zero = &data->t_zero;
	philo->mealcount = 0;
	philo->timetodie = data->timetodie;
	philo->timetoeat = data->timetoeat;
	philo->timetosleep = data->timetosleep;
	philo->end = &data->end;
	pthread_mutex_init(&philo->lasteatmutex, 0);
	pthread_mutex_init(&philo->meals, 0);
	philo->display = &data->display;
	philo->start = &data->start;	
	philo->fork1 = &data->forkmutex[((i / 2) * 2 + 1) % data->philnbr];
	philo->fork2 = &data->forkmutex[(((i + 1) / 2) * 2) % data->philnbr];
	
}

int	init_data(int ac, char **av, t_data *data)
{
	int	i;

	init_base_data(ac, av, data);
	if (!alloc_data(data))
		return (0);
	i = -1;
	pthread_mutex_init(&data->display, 0);
	pthread_mutex_init(&data->start, 0);
	while (++i < data->philnbr)
	{	
		data->mealcount[i] = 0;
		pthread_mutex_init(&data->forkmutex[i], 0);
	}
	i = -1;
	while (++i < data->philnbr)
		init_philo(data, &data->philo[i], i);	
	return (1);
}

void	init_t_zero(t_data *data)
{
	struct timeval	tv;
	int				i;

	i = -1;
	gettimeofday(&tv, 0);
	data->t_zero = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	while (++i < data->philnbr)
		data->philo[i].lasteat = data->t_zero;
}

int	main(int ac, char **av)
{
	t_data	data;
	int		i;

	if (ac < 5 || ac > 6)
		return (0);
	if (!init_data(ac, av, &data))
		return (0);
	i = -1;
	pthread_mutex_lock(&data.start);
	init_t_zero(&data);
	pthread_mutex_unlock(&data.start);
	while (++i < data.philnbr)		
		pthread_create(&data.tid[i], NULL, philo_live, &data.philo[i]);
	if (ender(&data, data.philo) == 0)
	{
		i = -1;
		while (++i < data.philnbr)
			pthread_join(data.tid[i], NULL);
		free_data(&data);
	}
	return (0);
}
