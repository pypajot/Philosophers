/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ppajot <ppajot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/25 20:24:04 by ppajot            #+#    #+#             */
/*   Updated: 2022/06/29 20:08:32 by ppajot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../philo.h"

int	ft_strcmp(char *str1, char *str2)
{
	while (*str1 && *str1 == *str2)
	{
		str1++;
		str2++;
	}
	return (*str1 - *str2);
}

void	display(long t_zero, int id, pthread_mutex_t *displaymutex, char *str)
{
	struct timeval	tv;

	gettimeofday(&tv, 0);
	pthread_mutex_lock(displaymutex);
	printf("%li %i %s\n", tv.tv_sec * 1000 + tv.tv_usec / 1000 - t_zero,
		id, str);
	if (ft_strcmp(str, "died"))
		pthread_mutex_unlock(displaymutex);
}

void	takes_put_fork(t_data *data, int id, int forknbr, int takes_put)
{
	int	fork;

	if (forknbr == 2)
		fork = ((id / 2) * 2) % data->philnbr;
	else
		fork = (((id - 1) / 2) * 2 + 1) % data->philnbr;
	pthread_mutex_lock(&data->forkmutex[fork]);
	data->forktab[fork] = takes_put;
	pthread_mutex_unlock(&data->forkmutex[fork]);
	if (takes_put == 0)
		display(data->t_zero, id, &data->display, "has taken a fork");
}

int	check_fork(t_data *data, int id, int forknbr)
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

int	check_last_eat(t_data *data, int i)
{
	int				res;
	struct timeval	tv;

	pthread_mutex_lock(&data->lasteatmutex);
	gettimeofday(&tv, 0);
	if (tv.tv_sec * 1000 + tv.tv_usec / 1000 - data->lasteat[i]
		> data->timetodie)
		res = 1;
	else
		res = 0;
	pthread_mutex_unlock(&data->lasteatmutex);
	return (res);
}

int	check_end(t_data *data)
{
	int	res;

	pthread_mutex_lock(&data->endmutex);
	if (data->end)
		res = 1;
	else if (!data->end)
		res = 0;
	pthread_mutex_unlock(&data->endmutex);
	return (res);
}

void	think(t_data *data, int id, int *eat_sleep_think)
{
	display(data->t_zero, id, &data->display, "is thinking");
	*eat_sleep_think = 0;
}

void	ft_sleep(t_data *data, int id, int *eat_sleep_think)
{
	display(data->t_zero, id, &data->display, "is sleeping");
	usleep(data->timetosleep * 1000);
	*eat_sleep_think = 3;
}

void	eat(t_data *data, int id, int *eat_sleep_think)
{
	struct timeval	tv;

	takes_put_fork(data, id, 2, 0);
	pthread_mutex_lock(&data->lasteatmutex);
	gettimeofday(&tv, 0);
	data->lasteat[id - 1] = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	pthread_mutex_unlock(&data->lasteatmutex);
	display(data->t_zero, id, &data->display, "is eating");
	pthread_mutex_lock(&data->meals);
	data->mealcount[id - 1]++;
	pthread_mutex_unlock(&data->meals);
	usleep(data->timetoeat * 1000);
	takes_put_fork(data, id, 1, 1);
	takes_put_fork(data, id, 2, 1);
	*eat_sleep_think = 2;
}

void	*philo(void *arg)
{
	t_data	*data;
	int		eat_sleep_think;
	int		id;

	data = (t_data *)arg;
	eat_sleep_think = 0;
	pthread_mutex_lock(&data->idmutex);
	id = ++data->philid;
	pthread_mutex_unlock(&data->idmutex);
	while (check_end(data))
	{
		if (eat_sleep_think == 3)
			think(data, id, &eat_sleep_think);
		else if (eat_sleep_think == 2)
			ft_sleep(data, id, &eat_sleep_think);
		else if (eat_sleep_think == 1 && check_fork(data, id, 2))
			eat(data, id, &eat_sleep_think);
		else if (eat_sleep_think == 0 && check_fork(data, id, 1))
		{
			takes_put_fork(data, id, 1, 0);
			eat_sleep_think = 1;
		}		
	}
	return (0);
}

void	zero_end(t_data *data)
{
	pthread_mutex_lock(&data->endmutex);
	data->end = 0;
	pthread_mutex_unlock(&data->endmutex);
}

int	ender(void *arg)
{
	t_data	*data;
	int		i;
	int		counter;

	data = (t_data *)arg;
	while (1)
	{
		i = -1;
		counter = 0;
		while (++i < data->philnbr)
		{
			pthread_mutex_lock(&data->meals);
			if (data->mealcount[i] >= data->mealnbr)
				counter++;
			pthread_mutex_unlock(&data->meals);
			if (counter == data->philnbr)
				return (zero_end(data), 0);
			if (check_last_eat(data, i))
			{
				display(data->t_zero, i + 1, &data->display, "died");
				return (zero_end(data), 0);
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
	free(data->forktab);
	free(data->mealcount);
	free(data->tid);
	free(data->lasteat);
	free(data->forkmutex);
}

int	alloc_data(t_data *data)
{
	data->forktab = (int *)malloc(sizeof(int) * data->philnbr);
	data->mealcount = (int *)malloc(sizeof(int) * data->philnbr);
	data->tid = (pthread_t *)malloc(sizeof(pthread_t) * data->philnbr);
	data->lasteat = (long *)malloc(sizeof(long) * data->philnbr);
	data->forkmutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)
			* data->philnbr);
	if (!data->forktab || !data->mealcount || !data->tid || !data->lasteat
		|| !data->forkmutex)
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

int	init_data(int ac, char **av, t_data *data)
{
	int	i;

	init_base_data(ac, av, data);
	if (!alloc_data(data))
		return (0);
	i = -1;
	while (++i < data->philnbr)
	{
		data->mealcount[i] = 0;
		data->forktab[i] = 1;
		pthread_mutex_init(&data->forkmutex[i], 0);
		data->lasteat[i] = data->t_zero;
	}
	pthread_mutex_init(&data->display, 0);
	pthread_mutex_init(&data->lasteatmutex, 0);
	pthread_mutex_init(&data->idmutex, 0);
	pthread_mutex_init(&data->endmutex, 0);
	pthread_mutex_init(&data->meals, 0);
	return (1);
}

int	main(int ac, char **av)
{
	t_data	data;
	int		i;

	if (ac < 5 || ac > 6)
		return (0);
	if (!init_data(ac, av, &data))
		return (0);
	i = 0;
	data.philid = 0;
	while (i < data.philnbr)
	{			
		pthread_create(&data.tid[i], NULL, philo, &data);
		usleep(50);
		i++;
	}
	if (ender(&data) == 0)
	{
		i = -1;
		pthread_mutex_unlock(&data.display);
		while (++i < data.philnbr)
			pthread_join(data.tid[i], NULL);
		free_data(&data);
		return (0);
	}
	return (0);
}
